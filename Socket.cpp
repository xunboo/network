#include "Socket.h"
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/ioctl.h>

using namespace std;

Socket::Socket()
{
    socket_Fd = -1;

    isBound = false;
    isClosed = false;
    isConnected = false;
    isAccepted = false;

    memset(&socketInfo, 0, sizeof(socketInfo));
}

Socket::~Socket()
{
}

const string Socket::getSocketInfoStr()
{
    stringstream ss;
    ss << inet_ntoa(socketInfo.sin_addr) << ":" << htons(socketInfo.sin_port);

    return ss.str();
}

const string Socket::getRemoteSocketStr(const sockaddr_in& remoteAddr)
{
    stringstream ss;
    ss << inet_ntoa(remoteAddr.sin_addr) << ":" << htons(remoteAddr.sin_port);

    return ss.str();
}