#include "AIBTcpServer.h"

using namespace aib::aibnet;

void func(AIBTcpServer *tcpServer, AIBTcpReadData *readdata)
{
    printf("recv data: %s\n", readdata->data());

    delete readdata;
}

int main(int argc, char **argv)
{
    try
    {
        AIBTcpServer tcpServer(AIBHostAddress::AnyIPv4, 10010);
        tcpServer.setCallbackFunc(func);
        tcpServer.loop();
    }
    catch(const std::exception& e)
    {
        printf("%s\n", e.what());
    }

    return 0;
}