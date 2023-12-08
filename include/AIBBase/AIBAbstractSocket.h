#ifndef __AIB_ABSTRACT_SOCKET_H__
#define __AIB_ABSTRACT_SOCKET_H__

#include "AIBObject.h"
#include "AIBTypes.h"
#include "AIBHostAddress.h"

#define AIBABSTRACTSOCKET_DEBUG

#if defined(__linux__)
#include <unistd.h>
#endif //

namespace aib
{
    namespace aibnet
    {
        class AIBAbstractSocket : public AIBObject
        {
            public:
                enum IP_Protocol
                {
                    IPV4Protocol=0x01,
                    IPV6Protocol=0x02
                };

                enum SocketType
                {
                    UnknownSocketType=-1,   // Other than TCP, UDP
                    TcpSocket=0,            // tcp
                    UdpSocket=1,            // udp
                };

                enum SocketState
                {
                    UnconnectedState=0,     // 
                    ConnectingState=1,      //
                    ConnectedState=2,       // 
                    ClosingState=3,         // 
                };

            public:
                AIBAbstractSocket(AIBAbstractSocket::SocketType socketType): AIBObject(), m_socket_type(socketType) {}
                virtual ~AIBAbstractSocket() { aibclose(); }

            public:
                // bool                bind(const AIBHostAddress &address, auint16 port=0);
                virtual bool        connectToHost(const std::string &hostname, auint16 port) 
                {
                    connectToHost(AIBHostAddress(hostname), port); 
                }

                virtual bool        connectToHost(const AIBHostAddress address, auint16 port)=0;
                bool                flush();
                
                aint                read(char *data, asize size)
                {
                    ::read(m_sockfd, data, size);
                }

                aint                write(const char *data, asize size)
                {
                    ::write(m_sockfd, data, size);
                }

            public:
                inline SocketType       socketType() const { return m_socket_type; }
                inline AIBHostAddress   localAddress() const { return m_host_address; }
                auint16                 localPort() const { return m_port; }
                inline SocketState      state() const { return m_socket_state; }

            private:
                inline void             setSocketType(AIBAbstractSocket::SocketType type) { m_socket_type=type; }
                inline void             setSocketState(AIBAbstractSocket::SocketState st) { m_socket_state=st; }
                inline void             aibclose()
                {
                    #if defined(__linux__)
                        if (close(m_sockfd) < 0)
                        {
#if defined AIBABSTRACTSOCKET_DEBUG
                            printf("close socket fd error: %s\n", std::strerror(errno));
#endif // 
                        }
                        else
                        {
                            m_socket_state=SocketState::ClosingState;
                        }
                    #endif // 
                }


            private:
                AIBSocket           m_sockfd;
                SocketType          m_socket_type;
                AIBHostAddress      m_host_address;
                auint16             m_port;
                SocketState         m_socket_state;
        };
    }
}

#endif // __AIB_ABSTRACT_SOCKET_H__