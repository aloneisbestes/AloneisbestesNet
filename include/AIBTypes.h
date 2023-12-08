#ifndef __AIB_TYPES_H__
#define __AIB_TYPES_H__

/**
 * @brief Data type definition
 */

namespace aib
{
    namespace aibtype
    {
        // Define base variable
        using aint8                 = char;
        using auint8                = unsigned char;
        using achar                 = char;
        using auchar                = unsigned char;
        using auint16               = unsigned short;
        using aint16                = short;
        using aint                  = int;
        using auint                 = unsigned;
        using auint32               = auint;
        using aint32                = aint;
        using aint64                = long long;
        using auint64               = unsigned long;
        using asize                 = auint32;
        using atype                 = aint32;
    }
}

/**
 * @brief Compatible with different underlying operating systems
*/
#if defined(__linux__)
#include <netinet/in.h>
#include <sys/socket.h>
#define     AIBSocket_t             int
#define     AIBEvent_t              int
#define     AIBEPOLL_SUPPORT        0x01        // support epoll mode

#ifndef     AIBEPOLL_MODE
#define     AIBEPOLL_MODE

enum AIBEpollMode
{
    EpollMode_IN=0x00,       // horizontal trigger
    EpollMode_ET=0x01        // edge trigger
};
#endif      // AIBEPOLL_MODE

#elif defined(_WIN32)

#define     AIBSocket_t             int
#define     AIBEvent_t              int
#define     AIBEPOLL_SUPPORT        0x00        // no support epoll mode
#endif

#define     AIBSocket               AIBSocket_t
#define     AIBEvent                AIBEvent_t
#define     AIBAddr                 struct sockaddr
#define     AIBAddrIn               struct sockaddr_in
#define     AIBAddrSize             sizeof(struct sockaddr)
#define     AIBAddrInSize           sizeof(struct sockaddr_in)

#endif // __AIB_TYPES_H__