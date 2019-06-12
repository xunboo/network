#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "Socket.h"
#include <set>

class TCPSocket : public Socket
{
public:
    TCPSocket();
    ~TCPSocket();

    TCPSocket(int tcp_socket, sockaddr_in* addr);

    bool isStreamSocket()
    {
        return true;
    }

    // use for server
public:
    int wait();
    void bindSocket();
    void closeSocket();
    TCPSocket* acceptSocket();
    void removeClientSocket(TCPSocket* pClientSocket);

    // use for client
public:
    void connect(const char* host, int port = TCP_PORT);
    int send(const char* buffer, int buf_size);
    int receive(char* buffer, int buf_size);
    void setServerSocket(TCPSocket* pTcpSocket)
    {
        serverSocket = pTcpSocket;
    }
    TCPSocket* getServerSocket()
    {
        return serverSocket;
    }

private:
    std::set<TCPSocket*> socketList;
    TCPSocket* serverSocket;
};

#endif // TCPSOCKET_H
