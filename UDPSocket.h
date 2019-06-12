#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "Socket.h"

class UDPSocket : public Socket
{
public:
    UDPSocket();
    ~UDPSocket();

    bool isStreamSocket()
    {
        return false;
    }

public:
    void bindSocket();
    void closeSocket();

    void connect(const char* host, int port = UDP_PORT);
    int send(const char* buffer, int buf_size);
    int receive(char* buffer, int buf_size);
    int wait();

public:
    // use for server call client side
    int sendToClient(const char* buffer, int buf_size, sockaddr_in& clientAddr);
    int recvFromClient(char* buffer, int buf_size, sockaddr_in& clientAddr);

public:
    struct sockaddr_in serverAddr; // for remote usage
};

#endif // UDPSOCKET_H
