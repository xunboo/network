#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>
#include "Constant.h"

class Socket
{
public:
    Socket();
    virtual ~Socket();

public:
    virtual void bindSocket() = 0;
    virtual void closeSocket() = 0;
    virtual int wait() = 0;

    virtual void connect(const char* host, int port) = 0;
    virtual int send(const char* buffer, int buf_size) = 0;
    virtual int receive(char* buffer, int buf_size) = 0;

    virtual bool isStreamSocket() = 0;

public:
    inline bool getConnected()
    {
        return isConnected;
    }
    inline bool getClosed()
    {
        return isClosed;
    }
    inline bool getAccepted()
    {
        return isAccepted;
    }
    inline const sockaddr_in getSocketInfo()
    {
        return socketInfo;
    }

    const std::string getRemoteSocketStr(const sockaddr_in& remoteAddr); // for client side
    const std::string getSocketInfoStr();          // own socket or incoming client socket each

protected:
    int socket_Fd;    // socket file descriptor
    int socket_Port;  // socket port
    bool isBound;     // bind state
    bool isConnected; // connect state
    bool isClosed;    // close state
    bool isAccepted;  // accepted state

    sockaddr_in socketInfo; // socket information
};

#endif // SOCKET_H
