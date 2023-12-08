#ifndef __AIB_TCP_DATA_H__
#define __AIB_TCP_DATA_H__

#include <cassert>
#include <cstring>
#include "AIBTypes.h"
#include "AIBHostAddress.h"

namespace aib
{
    namespace aibnet
    {
        class AIBTcpServer;

        /**
         * @brief This class stores data read from the client and information such as socket and ip ports
        */
        class AIBTcpData
        {
            public:
                struct AIBTcpConnInfo
                {
                    public:
                        AIBHostAddress  m_host_address;
                        aint            m_port;
                        AIBSocket       m_sockfd;

                        AIBTcpConnInfo(AIBSocket sockfd=-1, aint port=-1, AIBHostAddress address=AIBHostAddress::AnyIPv4)
                            : m_sockfd(m_sockfd), m_port(port), m_host_address(address)
                            {}

                        AIBTcpConnInfo(const AIBTcpConnInfo &other) { copy(other); }

                        AIBTcpConnInfo &operator=(const AIBTcpConnInfo &other)
                        {
                            copy(other);
                            return *this;
                        }

                    private:
                        friend class AIBTcpData;

                        void copy(const AIBTcpConnInfo &other)
                        {
                            m_host_address=other.m_host_address;
                            m_port=other.m_port;
                            m_sockfd=other.m_sockfd;
                        }
                };

            public:
                AIBTcpData() : m_data(nullptr), m_size(0) { }
                ~AIBTcpData() { if (m_data) delete [] m_data; }

                inline AIBSocket        socketfd() const { return m_connInfo.m_sockfd; }
                inline AIBHostAddress   hostAddredd() const { return m_connInfo.m_host_address; }
                inline aint             port() const { return m_connInfo.m_port; }

                inline const char *     data() const { return m_data; }
                inline asize            size() const { return m_size; }

                /**
                 * @brief The size passed in must be obtained through this class
                */
                inline void             data(char *data, asize size)
                {
                    assert(size <= m_size && size != 0 && "size <= m_size && size != 0");
                    assert(m_data && "m_data == nullprt");
                    std::memcpy(data, m_data, size);
                }

            protected:
                void                    setData(const char *data, asize size) 
                { 
                    if (m_data) delete [] m_data;
                    m_size=size;
                    m_data=new char[m_size+5];
                    std::memcpy(m_data, data, m_size);
                }

                void                    setSockfd(AIBSocket sockfd) { m_connInfo.m_sockfd=sockfd; }
                void                    setHostAddr(const AIBHostAddress &address) { m_connInfo.m_host_address=address; }
                void                    setPort(aint port) { m_connInfo.m_port=port; }
                void                    setConnInfo(const AIBTcpConnInfo &connInfo) { m_connInfo=connInfo; }

            private:
                friend class AIBTcpServer;

            private:
                AIBTcpConnInfo      m_connInfo;
                achar *             m_data;
                asize               m_size;
        };

        using AIBTcpReadData = AIBTcpData;
        using AIBTcpWriteData = AIBTcpData;
    }
}

#endif // __AIB_TCP_DATA_H__