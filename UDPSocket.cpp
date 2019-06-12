#include "UDPSocket.h"
#include "Socket.h"
#include "Constant.h"
#include "Utilities.h"
#include <arpa/inet.h>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <set>

using namespace std;

UDPSocket::UDPSocket()
{
    socket_Port = UDP_PORT;
    socket_Fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_Fd < 0)
    {
        throw std::runtime_error("Cannot create UDP Socket");
    }
}

UDPSocket::~UDPSocket()
{
    closeSocket();
}

void UDPSocket::bindSocket()
{
    if(isClosed)
    {
        throw std::runtime_error("Bind error, UDP socket is closed");
    }
    if(isBound)
    {
        throw std::runtime_error("UDP socket had been already bound");
    }

    socketInfo.sin_family = AF_INET;
    socketInfo.sin_addr.s_addr = INADDR_ANY;
    socketInfo.sin_port = htons(socket_Port);

    int enable = 1;
    if(::setsockopt(socket_Fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
    {
        FILE_LOG(logERROR) << "set SO_REUSEADDR error";
    }
    if(::setsockopt(socket_Fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)))
    {
        FILE_LOG(logERROR) << "set SO_REUSEPORT error";
    }

    if(::bind(socket_Fd, (sockaddr*)&socketInfo, sizeof(socketInfo)) < 0)
    {
        throw std::runtime_error("Cannot bind to port");
    }

    isBound = true;
    FILE_LOG(logINFO) << "UDP socket is bound";
    isConnected = true;
}

void UDPSocket::closeSocket()
{
    if(!isClosed)
    {
        ::shutdown(socket_Fd, SHUT_RDWR);
        ::close(socket_Fd);

        isClosed = true;
        isConnected = false;

        FILE_LOG(logINFO) << "UDP socket is closed";
    }
}

int UDPSocket::wait()
{
    fd_set rfds;
    struct timeval timeout;
    int retVal;

    FD_ZERO(&rfds);
    FD_SET(socket_Fd, &rfds);

    // Wait up to MAX_RECV_TIMEOUT seconds.

    timeout.tv_sec = MAX_RECV_TIMEOUT;
    timeout.tv_usec = 0;

    retVal = select(socket_Fd + 1, &rfds, NULL, NULL, &timeout);

    if(retVal == -1)
        return -1;
    else if(retVal)
        return 1;
    // FD_ISSET(0, &rfds) will be true.
    else
        return 0; // timeout;
}

void UDPSocket::connect(const char* host, int port)
{
    if(isClosed)
    {
        throw std::runtime_error("Cannot connect, socket is closed");
    }
    if(isConnected)
    {
        FILE_LOG(logERROR) << "Already connected to a remote host";
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host);

    int ret = ::connect(socket_Fd, (sockaddr*)&serverAddr, sizeof(serverAddr));

    if(ret < 0)
    {
        throw std::runtime_error("Cannot connect to remote host");
    }

    isConnected = true;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    ::getsockname(socket_Fd, (struct sockaddr*)&socketInfo, &addrlen);

    FILE_LOG(logINFO) << "UDP socket is connected to remote from " << getSocketInfoStr();
    ; // not real connected, just mean remote server is accessiable
}

int UDPSocket::send(const char* buffer, int buf_size)
{
    if(isClosed)
    {
        FILE_LOG(logERROR) << "Cannot send on UDP, socket is closed";
        return -1;
    }
    if(!isConnected)
    {
        FILE_LOG(logERROR) << "Cannot send on UDP, socket is not connected";
        return -1;
    }

    if(buffer == NULL || buf_size == 0)
    {
        FILE_LOG(logERROR) << "send UDP buffer is wrong";
        return -2;
    }

    // socklen_t addrlen = sizeof(struct sockaddr_in);
    // int sendBytes = ::sendto(socket_Fd, buffer, buf_size, 0, (struct sockaddr *) &serverAddr, addrlen);
    int sendBytes = ::send(socket_Fd, buffer, buf_size, 0);

    if(sendBytes <= 0)
    {
        FILE_LOG(logERROR) << "send UDP date error, socket drop";
        isConnected = false;
        return -3;
    }

    return sendBytes;
}

int UDPSocket::receive(char* buffer, int buf_size)
{
    if(isClosed)
    {
        FILE_LOG(logERROR) << "Cannot recv on UDP, socket is closed";
        return -1;
    }
    if(!isConnected)
    {
        FILE_LOG(logERROR) << "Cannot recv on UDP, socket is not connected";
        return -1;
    }

    if(buffer == NULL)
    {
        FILE_LOG(logERROR) << "recv UDP buffer is wrong";
        return -2;
    }

    // socklen_t addrlen = sizeof(struct sockaddr_in);
    // int recvBytes = ::recvfrom(socket_Fd, buffer, buf_size, 0, (struct sockaddr *) &serverAddr, &addrlen);
    int recvBytes = ::recv(socket_Fd, buffer, buf_size, 0);

    if(recvBytes < 0)
    {
        FILE_LOG(logERROR) << "recv UDP data error, socket drop";
        isConnected = false;
        return -3;
    }
    else if(recvBytes == 0)
    {
        FILE_LOG(logERROR) << "recv UDP data zero, socket close";
        isConnected = false;
    }

    if(recvBytes < 10)
    {
        FILE_LOG(logDEBUG1) << "UDP socket recv " << buffer;
    }

    return recvBytes;
}

int UDPSocket::sendToClient(const char* buffer, int buf_size, sockaddr_in& clientAddr)
{
    if(isClosed)
    {
        FILE_LOG(logERROR) << "Cannot send on UDP, client socket is closed";
        return -1;
    }
    if(!isConnected)
    {
        FILE_LOG(logERROR) << "Cannot send on UDP, client socket is not connected";
        return -1;
    }

    if(buffer == NULL || buf_size == 0)
    {
        FILE_LOG(logERROR) << "send UDP buffer is wrong";
        return -2;
    }

    socklen_t addrlen = sizeof(struct sockaddr_in);
    int sendBytes = ::sendto(socket_Fd, buffer, buf_size, 0, (struct sockaddr*)&clientAddr, addrlen);

    if(sendBytes <= 0)
    {
        FILE_LOG(logERROR) << "send UDP date error, client socket drop";
        isConnected = false;
        return -3;
    }

    return sendBytes;
}

int UDPSocket::recvFromClient(char* buffer, int buf_size, sockaddr_in& clientAddr)
{
    if(isClosed)
    {
        FILE_LOG(logERROR) << "Cannot recv on UDP, client socket is closed";
        return -1;
    }
    if(!isConnected)
    {
        FILE_LOG(logERROR) << "Cannot recv on UDP, client socket is not connected";
        return -1;
    }

    if(buffer == NULL)
    {
        FILE_LOG(logERROR) << "recv UDP buffer is wrong";
        return -2;
    }

    socklen_t addrlen = sizeof(struct sockaddr_in);
    int recvBytes = ::recvfrom(socket_Fd, buffer, buf_size, 0, (struct sockaddr*)&clientAddr, &addrlen);

    if(recvBytes < 0)
    {
        FILE_LOG(logERROR) << "recv UDP data error, client socket drop";
        isConnected = false;
        return -3;
    }
    else if(recvBytes == 0)
    {
        FILE_LOG(logERROR) << "recv UDP data zero, client socket close";
        isConnected = false;
    }

    if(recvBytes < 10)
    {
        FILE_LOG(logDEBUG1) << "UDP socket recv " << buffer << " from remote";
    }

    return recvBytes;
}