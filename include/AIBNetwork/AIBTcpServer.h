#ifndef __AIB_TCP_SERVER_H__
#define __AIB_TCP_SERVER_H__

#include <cassert>
#include <unordered_map>

#include "AIBObject.h"
#include "AIBException.h"
#include "AIBTypes.h"
#include "AIBAbstractSocket.h"
#include "AIBHostAddress.h"
#include "AIBTcpData.h"

#define AIBTCPSERVER_DEBUG

namespace aib
{
    namespace aibnet
    {
        using namespace aib::aibtype;

        class AIBTcpServer : public AIBObject, public AIBNoncopyable
        {
            public:
                /**
                 * @brief readData need manual tripping
                */
                typedef void (*__callback_read_func)(AIBTcpServer *tcpServer, AIBTcpReadData *data);

                /**
                 * @brief Disable the client socket callback function
                */
                typedef void (*__callback_close_func)(AIBTcpData::AIBTcpConnInfo connInfo);

                /**
                 * @brief Connected the client socket callback function
                */
                typedef void (*__callback_conn_func)(AIBTcpData::AIBTcpConnInfo connInfo);

            private:
                /**
                 * @brief Tcp Client information: ip, port, sockfd
                */
                using TcpClientInfo = AIBTcpData::AIBTcpConnInfo;

                static const asize  __listen_size=10;
                const static asize  __size=4096;

            public:
                AIBTcpServer(AIBHostAddress address=AIBHostAddress::AnyIPv4, aint port=0);
                ~AIBTcpServer();

                /**
                 * @brief Callback user-defined function when the socket receives data, data is 
                 * The data pointer needs to be deleted by the user using "delete"
                 * @param callback_func -> void (*__callback_read_func)(AIBTcpServer *tcpServer, AIBTcpReadData *data)
                */
                inline void             setCallbackFunc(__callback_read_func callback_func) { m_read_func=callback_func; }

                /**
                 * @brief Callback user-defined function when socket closes
                 * @param callback_func -> void (*__callback_close_func)(AIBTcpData::AIBTcpConnInfo connInfo)
                */
                inline void             setCloseCallbackFunc(__callback_close_func callback_func) { m_close_func=callback_func; }

                /**
                 * @brief Add socket Output data needs to be sent
                 * @param sockfd The socket that needs to send data
                 * @param dataPtr Data that needs to be sent
                 * @param dataSize The size of the sent data
                 * @return void
                 */
                void                    addOutputData(int sockfd, void *dataPtr, asize dataSize);

                /**
                 * @brief Removes closed sockets from the online client collection
                 * @param sockfd Client socket to be removed
                 * @return void
                */
                void                    deleteOnline(AIBSocket sockfd);

            public:
                void                    loop();

            protected:
                virtual void            run();
                virtual AIBTcpReadData* recv(AIBSocket sockfd);
                AIBSocket               aibsockt();
                void                    aibclose();
                void                    aibclose(AIBSocket sockfd);
                void                    aibbind();
                void                    aiblisten();
                void                    setnonblocking(AIBSocket fd);
                void                    setListenfdReuse(AIBSocket fd);

            private:
                #ifdef __linux__
                void addEpollEvent(AIBSocket sockfd);
                void delEpollEvent(AIBSocket sockfd);
                void delEpollOut(int sockfd);
                #endif // __linux__

            private:
                AIBSocket           m_listenfd;
                AIBSocket           m_clientfd;
                AIBHostAddress      m_host_address;
                aint                m_port;
                AIBEvent            m_eventfd;
                achar               m_read_buffer[__size];

                // client fd map
                std::unordered_map<AIBSocket, TcpClientInfo> m_online;


                // Handling callback functions
                __callback_read_func        m_read_func;
                __callback_close_func       m_close_func;
                __callback_conn_func        m_conn_func;
        };
    }
}


#endif // __AIB_TCP_SERVER_H__