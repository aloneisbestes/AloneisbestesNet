#ifndef __AIB_TCPSOCKET_H__
#define __AIB_TCPSOCKET_H__

#include "AIBAbstractSocket.h"

namespace aib
{
    namespace aibnet
    {
        class AIBTcpSocket : public AIBAbstractSocket
        {
            public:
                AIBTcpSocket(): AIBAbstractSocket(AIBAbstractSocket::TcpSocket) {}
                ~AIBTcpSocket() {}

                virtual bool        connectToHost(const AIBHostAddress address, auint16 port);

            private:

        };
    }
}

#endif // __AIB_TCPSOCKET_H__