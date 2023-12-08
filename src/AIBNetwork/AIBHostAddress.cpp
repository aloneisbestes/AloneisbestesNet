#include "AIBHostAddress.h"
#include <cassert>
#include <regex>

#if defined(__linux__)
#include <arpa/inet.h>
#endif // 

using namespace aib::aibnet;

aib::aibnet::AIBHostAddress::AIBHostAddress()
    : m_ipstr(""), m_ipnum(0), m_isnull(true), m_isbroadcast(false),
      m_protocol(AIBAbstractSocket::IPV4Protocol)
{}

aib::aibnet::AIBHostAddress::AIBHostAddress(AIBHostAddress::SpecialAddress address)
    : AIBHostAddress()
{
    switch (address)
    {
        case SpecialAddress::Null:
        {
            m_isnull=true;
            m_special_addr=Null;
        }
        break;
        case SpecialAddress::LocalHost:
        {
            m_ipstr="127.0.0.1";
            m_ipnum=inet_addr(m_ipstr.c_str());
            m_protocol=AIBAbstractSocket::IPV4Protocol;
            m_special_addr=LocalHost;
            m_isnull=false;
        }
        break;
        case SpecialAddress::Broadcast:
        {
            m_ipstr="255.255.255.255";
            m_ipnum=inet_addr(m_ipstr.c_str());
            m_isbroadcast=true;
            m_protocol=AIBAbstractSocket::IPV4Protocol;
            m_special_addr=Broadcast;
            m_isnull=false;
        }
        break;
        case SpecialAddress::AnyIPv4:
        {
            m_ipstr="0.0.0.0";
            m_ipnum=INADDR_ANY;
            m_protocol=AIBAbstractSocket::IPV4Protocol;
            m_special_addr=AnyIPv4;
            m_isnull=false;
        }
        break;
        default:
        {
            m_special_addr=Null;
            assert(0 && "SpecialAddress is error");
        }
        break;
    }
}

aib::aibnet::AIBHostAddress::AIBHostAddress(const AIBHostAddress &address)
{
    copy(address);
}

aib::aibnet::AIBHostAddress::AIBHostAddress(const std::string &address)
    : AIBHostAddress()
{
    setAddress(address);
}

aib::aibnet::AIBHostAddress::AIBHostAddress(const sockaddr *sockaddr)
{
    setAddress(sockaddr);
}

aib::aibnet::AIBHostAddress::AIBHostAddress(auint32 ip4Addr)
{
    setAddress(ip4Addr);
}

aib::aibnet::AIBHostAddress::~AIBHostAddress()
{
}

bool aib::aibnet::AIBHostAddress::setAddress(const std::string &address)
{
    // Determine whether it is ipv4 or ipv6
    if (isIPV4(address))
    {
        m_protocol=AIBAbstractSocket::IPV4Protocol;
        m_isnull=false;
        m_ipnum=inet_addr(address.c_str());
        m_ipstr=address;
    }
#ifndef _NO_IPV6_
    else if (isIPV6(address))
    {
        m_protocol=AIBAbstractSocket::IPV6Protocol;
        m_isnull=true;
        // TODO: ipv6 change
        // .....
    }
#endif // _NO_IPV6_
    else 
    {
        m_special_addr=Null;
        m_isnull=true;
        return false;
    }
    return true;
}

void aib::aibnet::AIBHostAddress::setAddress(auint32 ip4Addr)
{
    m_ipnum=ip4Addr;
    m_isnull=false;
    m_ipstr=auint32ToIPAddress(ip4Addr);
    if (m_ipstr == "255.255.255.255")
    {
        m_special_addr=Broadcast;
        m_isbroadcast=true;
    }
}

void aib::aibnet::AIBHostAddress::setAddress(const sockaddr *sockaddr)
{
    assert(sockaddr && "sockaddr not is nullprt"); 
    if (sockaddr->sa_family == AF_INET)
    {
        const sockaddr_in* sa4 = reinterpret_cast<const sockaddr_in*>(sockaddr);
        char ip4[INET_ADDRSTRLEN+5];
        std::memset(ip4, 0, sizeof(INET_ADDRSTRLEN+5));
        inet_ntop(AF_INET, &(sa4->sin_addr), ip4, INET_ADDRSTRLEN);
        m_ipstr=ip4;
        m_ipnum=inet_addr(m_ipstr.c_str());
    }
#ifndef _NO_IPV6_
    else if (sockaddr->sa_family == AF_INET6)
    {
        /*
        const sockaddr_in6* sa6 = reinterpret_cast<const sockaddr_in6*>(sockaddr);
        char ip6[INET6_ADDRSTRLEN+5];
        std::memset(ip6, 0, sizeof(INET6_ADDRSTRLEN+5));
        inet_ntop(AF_INET, &(sa6->sin6_addr), ip6, INET6_ADDRSTRLEN);
        */
    }
#endif // _NO_IPV6_
    else 
    {
        m_isnull=true;
    }
}

void aib::aibnet::AIBHostAddress::copy(const AIBHostAddress &address)
{
    m_ipnum=address.m_ipnum;
    m_ipstr=address.m_ipstr;
    m_isbroadcast=address.m_isbroadcast;
    m_isnull=address.m_isnull;
    m_protocol=address.m_protocol;
}

bool aib::aibnet::AIBHostAddress::isValidIPAddress(const std::string &address)
{
    if (m_protocol == AIBAbstractSocket::IPV4Protocol)
    {
        return isIPV4(address);
    }
#ifndef _NO_IPV6_
    else if (m_protocol == AIBAbstractSocket::IPV6Protocol)
    {
        return isIPV6(address);
    }
#endif // _NO_IPV6_

    return false;
}

void aib::aibnet::AIBHostAddress::init()
{
    m_ipnum=0;
    m_ipstr="";
    m_isbroadcast=false;
    m_isnull=true;
    m_protocol=AIBAbstractSocket::IPV4Protocol;
}

bool aib::aibnet::AIBHostAddress::isIPV4(const std::string &address)
{
    // IPv4 address regular expression
    const std::regex ipv4Regex(R"(\b(?:\d{1,3}\.){3}\d{1,3}\b)");
    return std::regex_match(address, ipv4Regex);
}

#ifndef _NO_IPV6_
bool aib::aibnet::AIBHostAddress::isIPV6(const std::string &address)
{
    // IPv6 address regular expression
    const std::regex ipv6Regex(R"(\b(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}\b)");
    return std::regex_match(address, ipv6Regex);
}
#endif // _NO_IPV6_

std::string aib::aibnet::AIBHostAddress::auint32ToIPAddress(auint32 ip4Addr)
{
    struct in_addr addr;
    addr.s_addr = htonl(ip4Addr);
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, ipStr, INET_ADDRSTRLEN);
    return ipStr;
}

bool aib::aibnet::AIBHostAddress::operator!=(const AIBHostAddress &other) const
{
    if (m_ipnum != other.m_ipnum)
        return true;
    if (m_ipstr != other.m_ipstr)
        return true;
    if (m_isbroadcast != other.m_isbroadcast)
        return true;
    if (m_isnull != other.m_isnull)
        return true;
    if (m_special_addr != other.m_special_addr)
        return true;
    return false;
}

bool aib::aibnet::AIBHostAddress::operator!=(AIBHostAddress::SpecialAddress other) const
{
    return m_special_addr != other;
}

AIBHostAddress &aib::aibnet::AIBHostAddress::operator=(AIBHostAddress::SpecialAddress address)
{
    *this=AIBHostAddress(address);
    return *this;
}

AIBHostAddress &aib::aibnet::AIBHostAddress::operator=(const AIBHostAddress &address)
{
    copy(address);
    return *this;
}

bool aib::aibnet::AIBHostAddress::operator==(const AIBHostAddress &other) const
{
    return !(*this != other);
}

bool aib::aibnet::AIBHostAddress::operator==(AIBHostAddress::SpecialAddress other) const
{
    return m_special_addr==other;
}
