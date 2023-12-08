#ifndef __AIB_HOST_ADDRESS_H__
#define __AIB_HOST_ADDRESS_H__

#include <string>
#include "AIBTypes.h"
#include "AIBAbstractSocket.h"

#define _NO_IPV6_       // ipv6 macros are not supported

namespace aib
{
    namespace aibnet
    {
        using namespace aib::aibtype;
        class AIBTcpServer;

        class AIBHostAddress
        {
            public:
                enum SpecialAddress 
                {
                    Null,
                    Broadcast,
                    LocalHost,
                    AnyIPv4,
                    #ifndef _NO_IPV6_
                    Any,
                    LocalHostIPv6,
                    AnyIPv6
                    #endif 
                };

                inline bool             isNull() const noexcept { return m_isnull; }

                inline std::string      toString() const { return m_ipstr; }
                inline const char *     toCString() const { return m_ipstr.c_str(); }

            public:
                /**
                 * @brief AIBHostAddress ipv6 is not currently supported
                 */
                AIBHostAddress();
                AIBHostAddress(AIBHostAddress::SpecialAddress address);
                AIBHostAddress(const AIBHostAddress &address);
                AIBHostAddress(const std::string &address);
                AIBHostAddress(const sockaddr *sockaddr);
                AIBHostAddress(auint32 ip4Addr);
                ~AIBHostAddress();

            public:
                bool setAddress(const std::string &address);
                void setAddress(auint32 ip4Addr);
                void setAddress(const sockaddr *sockaddr);

                #ifndef _NO_IPV6_
                void setAddress(quint8 *ip6Addr);
                void setAddress(const quint8 *ip6Addr);
                // void setAddress(const Q_IPV6ADDR &ip6Addr);
                #endif // _NO_IPV6_

                inline AIBAbstractSocket::IP_Protocol protocol() const { return m_protocol; }

                
                bool operator!=(const AIBHostAddress &other) const;
                bool operator!=(AIBHostAddress::SpecialAddress other) const;
                AIBHostAddress &operator=(AIBHostAddress::SpecialAddress address);
                AIBHostAddress &operator=(const AIBHostAddress &address);
                bool operator==(const AIBHostAddress &other) const;
                bool operator==(AIBHostAddress::SpecialAddress other) const;

            private:
                void            copy(const AIBHostAddress &address);
                bool            isValidIPAddress(const std::string &address);
                void            init();
                bool            isIPV4(const std::string &address);
                bool            isIPV6(const std::string &address);
                std::string     auint32ToIPAddress(auint32 ip4Addr);

            private:
                friend class AIBTcpServer;

            private:
                std::string                         m_ipstr;
                auint32                             m_ipnum;
                bool                                m_isnull;
                bool                                m_isbroadcast;  // Whether to broadcast ip
                AIBAbstractSocket::IP_Protocol      m_protocol;
                SpecialAddress                      m_special_addr;
        };
    }
}

#endif // __AIB_HOST_ADDRESS_H__