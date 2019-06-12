#include "TCPSocket.h"
#include "Constant.h"
#include "Utilities.h"
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <set>

using namespace std;

TCPSocket::TCPSocket()
{
    socket_Port = TCP_PORT;
    socket_Fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_Fd < 0)
    {
        throw std::runtime_error("Cannot create TCP Socket");
    }

    /*struct timeval timeout;
    timeout.tv_sec = MAX_RECV_TIMEOUT;
    timeout.tv_usec = 0;

    if (setsockopt (socket_Fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0){
        FILE_LOG(logERROR) << "setsockopt SO_RCVTIMEO failed\n";
    }*/

    /*if (setsockopt (socket_Fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0){
        FILE_LOG(logERROR) << "setsockopt SO_RCVTIMEO failed\n";
    }*/
}

TCPSocket::TCPSocket(int tcp_socket, sockaddr_in* addr)
{
    socket_Fd = tcp_socket;
    socketInfo.sin_family = addr->sin_family;
    socketInfo.sin_port = addr->sin_port;
    socketInfo.sin_addr = addr->sin_addr;

    isConnected = true;
    isAccepted = true; // incoming client socket

    serverSocket = NULL;
}

TCPSocket::~TCPSocket()
{
    if(isBound)
    {
        FILE_LOG(logINFO) << "delete " << socketList.size() << " clients sockets";
        for(set<TCPSocket*>::iterator itr = socketList.begin(); itr != socketList.end(); ++itr)
        {
            TCPSocket* pClient = *itr;
            delete pClient;
        }

        socketList.clear();
    }

    closeSocket();
}

void TCPSocket::bindSocket()
{
    if(isClosed)
    {
        throw std::runtime_error("Bind error, TCP socket is closed");
    }
    if(isBound)
    {
        throw std::runtime_error("TCP socket had been already bound");
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

    FILE_LOG(logINFO) << "TCP socket is bound";

    ::listen(socket_Fd, MAX_LISTEN_COUNT);
    isBound = true;

    FILE_LOG(logINFO) << "TCP socket is listening";
}

void TCPSocket::closeSocket()
{
    if(!isClosed)
    {
        // u_long iMode = 0;
        //::ioctl(socket_Fd, FIONBIO, &iMode);

        ::shutdown(socket_Fd, SHUT_RDWR);
        ::close(socket_Fd);

        isClosed = true;

        FILE_LOG(logINFO) << "TCP socket is closed";
    }
}

TCPSocket* TCPSocket::acceptSocket()
{
    if(!isBound)
    {
        throw std::runtime_error("Cannot accept, socket is not bound");
    }
    if(isClosed)
    {
        throw std::runtime_error("Cannot accept, socket is closed");
    }

    int client_fd;
    struct sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(struct sockaddr_in);

    client_fd = ::accept(socket_Fd, (sockaddr*)&clientAddr, &clientSize);
    if(client_fd < 0)
    {
        FILE_LOG(logERROR) << "Cannot accept connection";
        isClosed = true;
        return NULL;
    }

    // TCP socket used as Server (isBound == true) should delte it after disconnection
    TCPSocket* pClientSocket = new TCPSocket(client_fd, &clientAddr);
    pClientSocket->setServerSocket(this);
    socketList.insert(pClientSocket);

    FILE_LOG(logINFO) << "Server accepted a incoming connection from ip " << inet_ntoa(clientAddr.sin_addr) << ":"
                       << htons(clientAddr.sin_port);
    return pClientSocket;
}

int TCPSocket::wait()
{
    // for TCP, recv is enough to check if connection drop
    return 1;
}

void TCPSocket::connect(const char* host, int port)
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

    struct sockaddr_in serverAddr;
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

    FILE_LOG(logINFO) << "TCP socket is connected to remote from " << getSocketInfoStr();
}

int TCPSocket::send(const char* buffer, int buf_size)
{
    if(isClosed)
    {
        FILE_LOG(logERROR) << "Cannot send on TCP, socket is closed";
        return -1;
    }
    if(!isConnected)
    {
        FILE_LOG(logERROR) << "Cannot send on TCP, socket is not connected";
        return -1;
    }

    if(buffer == NULL || buf_size == 0)
    {
        FILE_LOG(logERROR) << "send TCP buffer is wrong";
        return -2;
    }

    int sendBytes = ::send(socket_Fd, buffer, buf_size, 0);

    if(sendBytes <= 0)
    {
        FILE_LOG(logERROR) << "send TCP date error, socket drop";
        isConnected = false;
        return -3;
    }

    return sendBytes;
}

int TCPSocket::receive(char* buffer, int buf_size)
{
    if(isClosed)
    {
        FILE_LOG(logERROR) << "Cannot recv on TCP, socket is closed";
        return -1;
    }
    if(!isConnected)
    {
        FILE_LOG(logERROR) << "Cannot recv on TCP, socket is not connected";
        return -1;
    }

    if(buffer == NULL)
    {
        FILE_LOG(logERROR) << "recv TCP buffer is wrong";
        return -2;
    }

    int recvBytes = ::recv(socket_Fd, buffer, buf_size, 0);

    if(recvBytes < 0)
    {
        FILE_LOG(logERROR) << "recv TCP data error, socket drop";
        isConnected = false;
        return -3;
    }
    else if(recvBytes == 0)
    {
        FILE_LOG(logERROR) << "recv TCP data zero, socket close";
        isConnected = false;
    }

    if(recvBytes < 10)
    {
        FILE_LOG(logDEBUG1) << "TCP socket recv " << buffer;
    }

    return recvBytes;
}

void TCPSocket::removeClientSocket(TCPSocket* pClientSocket)
{
    assert(pClientSocket != NULL);
    assert(socketList.size() != 0);

    if(pClientSocket)
    {
        delete pClientSocket;
        int ret = socketList.erase(pClientSocket);
        assert(ret == 1);
        pClientSocket = NULL;
    }
}