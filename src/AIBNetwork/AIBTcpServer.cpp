#include "AIBTcpServer.h"
#include "AIBException.h"
#if defined(__linux__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#endif // 

using namespace aib::aibexce;

aib::aibnet::AIBTcpServer::AIBTcpServer(AIBHostAddress address, aint port)
    : m_host_address(address), m_port(port)
{
    if (address.isNull())
    {
        setErrorString("address is null.");
        throw AIBException("AIBTcpServer constructor: %s", errstr().c_str());
    }

    m_conn_func=nullptr;
    m_close_func=nullptr;
    m_read_func=nullptr;

    aibsockt();
    setListenfdReuse(m_listenfd);
}

aib::aibnet::AIBTcpServer::~AIBTcpServer()
{
    aibclose();
}

void aib::aibnet::AIBTcpServer::addOutputData(int sockfd, void *dataPtr, asize dataSize)
{
#if defined(__linux__)
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLET|EPOLLOUT|EPOLLRDHUP;
    ev.data.ptr = dataPtr;
    ev.data.u32 = dataSize;
    if (epoll_ctl(m_eventfd, EPOLL_CTL_MOD, sockfd, &ev)) 
    {
        setErrorString(std::strerror(errno));
        throw AIBException("epoll ctl mod, errstr: %s", errstr().c_str());
    }
#endif // 
}

void aib::aibnet::AIBTcpServer::deleteOnline(AIBSocket sockfd)
{
    auto it=m_online.find(sockfd);

    if (it != m_online.end())
    {
#if defined(AIBTCPSERVER_DEBUG)
        printf("disconnect ip: %s, port: %d, socket: %d\n", it->second.m_host_address.toString().c_str(), 
            it->second.m_port, it->second.m_sockfd);
#endif //
        if (m_close_func != nullptr)
            m_close_func(it->second);
        // Delete to online list
        m_online.erase(it);
    }
    delEpollEvent(sockfd);
    aibclose(sockfd);
}

void aib::aibnet::AIBTcpServer::loop()
{
    aibbind();
    aiblisten();

#if defined(__linux__)
    // epoll is used on linux
    m_eventfd=epoll_create(1);
    if (m_eventfd < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("create epollfd, errstr: %s", errstr().c_str());
    }

    // Add the listening socket to epollfd
    epoll_event ev;
    std::memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd=m_listenfd;
    ev.events=EPOLLIN;
    if (epoll_ctl(m_eventfd, EPOLL_CTL_ADD, m_listenfd, &ev) < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("epoll ctl add, errstr: %s", errstr().c_str());
    }
#else 

#endif // 

    run();
}

void aib::aibnet::AIBTcpServer::run()
{
#if defined(__linux__)
    // linux 
    static aint16 ev_size=10;
    epoll_event epoll_events[ev_size];
    int nfds=0;
    AIBAddrIn client;
    socklen_t len=AIBAddrInSize;
    socklen_t clientLen=0;
#if defined(AIBTCPSERVER_DEBUG)
    printf("tcp server start run...........\n");
#endif // 
    for (;;)
    {
        nfds=epoll_wait(m_eventfd, epoll_events, ev_size, -1);
        if (nfds < 0)
        {
            continue;
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (epoll_events[i].data.fd == m_listenfd) // Listen to the socket
            {
                clientLen=len;
                memset(&client, 0, clientLen);
                m_clientfd=accept(m_listenfd, reinterpret_cast<AIBAddr*>(&client), &clientLen);
                if (m_clientfd < 0)
                {
                    continue;
                }

                // Obtain client information
                AIBTcpData::AIBTcpConnInfo clientInfo;
                clientInfo.m_host_address=AIBHostAddress(reinterpret_cast<AIBAddr*>(&client));
                clientInfo.m_port=ntohs(client.sin_port);
                clientInfo.m_sockfd=m_clientfd;
#if defined(AIBTCPSERVER_DEBUG)
                printf("connect ip: %s, port: %d, sockfd: %d\n", clientInfo.m_host_address.toCString(),
                    clientInfo.m_port, clientInfo.m_sockfd);
#endif // 
                try
                {         
                    setnonblocking(m_clientfd);
                    // clientfd add to epollfd
                    addEpollEvent(m_clientfd);
                    // Add to online list
                    m_online[m_clientfd]=clientInfo;
                    if (m_conn_func != nullptr)
                    {
                        m_conn_func(clientInfo);
                    }
                }
                catch(const std::exception& e)
                {
#if defined(AIBTCPSERVER_DEBUG)
                    printf("exception close sockfd: %d, errstr: %s\n", m_clientfd, e.what());
#endif // 
                    aibclose(m_clientfd);
                }
            }
            else if ((epoll_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)))
            {
                deleteOnline(epoll_events[i].data.fd);
            }
            else if (epoll_events[i].events & EPOLLIN || epoll_events[i].events & EPOLLET)
            {
                AIBSocket sockfd=epoll_events[i].data.fd;
                if (sockfd <= 2)
                {
                    continue;
                }

#if defined(AIBTCPSERVER_DEBUG)
                auto clienInfo=m_online[sockfd];
                printf("recv from ip: %s, prot: %d, sockfd: %d\n", clienInfo.m_host_address.toCString(),
                    clienInfo.m_port, sockfd);
#endif // 
                AIBTcpReadData *read_data_prt=recv(sockfd);
                if (read_data_prt != nullptr)
                {
#if defined(AIBTCPSERVER_DEBUG)
                printf("recv size: %d\n", read_data_prt->size());
#endif // 
                    m_read_func(this, read_data_prt);
                }
            }
            else if (epoll_events[i].events & EPOLLOUT)
            {
                
            }
            else 
            {
#if defined(AIBTCPSERVER_DEBUG)
                printf("epoll_events[i].events: %d, not find branch", epoll_events[i].events);
#endif //
            }
        }
    }

#endif // 
}

aib::aibnet::AIBTcpReadData *aib::aibnet::AIBTcpServer::recv(AIBSocket sockfd)
{
    int recv_size=0;
    std::string recv_data;
    do {
        memset(m_read_buffer, 0, __size);
        recv_size=::recv(sockfd, m_read_buffer, __size, 0);
        int tmpErrno=errno;
        if (recv_size < 0) 
        {
            // If the error is EAGAIN or EWOULDBLOCK, the current operation is unavailable and try again later
            if (tmpErrno == EAGAIN || tmpErrno == EWOULDBLOCK) 
            {
                break;
            }
            deleteOnline(sockfd);
        }
        else if (recv_size == 0)
        {
            deleteOnline(sockfd);
        }
        else 
        {
            recv_data.append(m_read_buffer, recv_size);
        }
    } while(1);

    auto clienInfo=m_online[sockfd];
    if (recv_data.size() == 0)
    {
#if defined(AIBTCPSERVER_DEBUG)
        printf("recv from ip: %s, prot: %d, sockfd: %d, read data size is: %ld\n", clienInfo.m_host_address.toCString(),
            clienInfo.m_port, sockfd, recv_data.size());
#endif 
        return nullptr;
    }

    AIBTcpReadData *tcpReadData=new AIBTcpReadData();
    if (tcpReadData != nullptr)
    {
        tcpReadData->setConnInfo(clienInfo);
        tcpReadData->setData(recv_data.c_str(), recv_data.size());
    }
        
    return tcpReadData;
}

AIBSocket aib::aibnet::AIBTcpServer::aibsockt()
{
    int domain=m_host_address.protocol() == AIBAbstractSocket::IPV4Protocol ? AF_INET : AF_INET6;
    m_listenfd=socket(domain, SOCK_STREAM, 0);
    if (m_listenfd < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("create socket, errstr: %s", errstr().c_str());
    }

    return m_listenfd;
}

void aib::aibnet::AIBTcpServer::aibbind()
{
    AIBAddrIn server_addr;
    std::memset(&server_addr, 0, AIBAddrInSize);

#if defined(AIBTCPSERVER_DEBUG)
    if (m_host_address.protocol() == AIBAbstractSocket::IPV4Protocol)
    {
        printf("bind tcp is ipv4\n");
    }
    else 
    {
        printf("bind tcp is ipv6\n");
    }
#endif // 

    server_addr.sin_family=(m_host_address.protocol() == AIBAbstractSocket::IPV4Protocol) ? AF_INET : AF_INET6;
    server_addr.sin_addr.s_addr=m_host_address.m_ipnum;
    server_addr.sin_port=htons(m_port);

    int retnum=bind(m_listenfd, reinterpret_cast<AIBAddr*>(&server_addr), AIBAddrInSize);
    if (retnum < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("bind socket, errstr: %s", errstr().c_str());
    }
}

void aib::aibnet::AIBTcpServer::aiblisten()
{
    int retnum=listen(m_listenfd, __listen_size);
    if (retnum < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("listen socket, errstr: %s", errstr().c_str());
    }
}

void aib::aibnet::AIBTcpServer::aibclose()
{
    aibclose(m_listenfd);
}

void aib::aibnet::AIBTcpServer::aibclose(AIBSocket sockfd)
{
#if defined(__linux__)
    if (close(sockfd) < 0)
    {
#if defined AIBTCPSERVER_DEBUG
        printf("close socket fd error: %s\n", std::strerror(errno));
#endif // 
    }
#endif // 
}

void aib::aibnet::AIBTcpServer::setnonblocking(AIBSocket fd)
{
#if defined(__linux__)
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("get mode fd: %d flags error, errstr: %s", fd, errstr().c_str());
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("change mode fd: %d flags error, errstr: %s", fd, errstr().c_str());
    }
#else
// TODO: windows setnoblocking
#error "no setnonblocking"
#endif // 
}

void aib::aibnet::AIBTcpServer::setListenfdReuse(AIBSocket fd)
{
#if defined(__linux__)
    int flag=1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
#else 
// TODO : windows reuse
#error "no so"
#endif // 
}

#if defined(__linux__)
void aib::aibnet::AIBTcpServer::addEpollEvent(AIBSocket sockfd)
{
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLET|EPOLLIN|EPOLLRDHUP;
    if (epoll_ctl(m_eventfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) 
    {
        setErrorString(std::strerror(errno));
        throw AIBException("epoll ctl add, errstr: %s", errstr().c_str());
    }
}
void aib::aibnet::AIBTcpServer::delEpollEvent(AIBSocket sockfd)
{
    if (epoll_ctl(m_eventfd, EPOLL_CTL_DEL, sockfd, nullptr) < 0)
    {
        setErrorString(std::strerror(errno));
        throw AIBException("epoll ctl del, errstr: %s", errstr().c_str());
    }
}
void aib::aibnet::AIBTcpServer::delEpollOut(AIBSocket sockfd)
{
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLET|EPOLLIN|EPOLLRDHUP;
    if (epoll_ctl(m_eventfd, EPOLL_CTL_MOD, sockfd, &ev) < 0) 
    {
        setErrorString(std::strerror(errno));
        throw AIBException("epoll ctl mod, errstr: %s", errstr().c_str());
    }
}
#endif //
