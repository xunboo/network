#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include "Constant.h"

class Server;
class Socket;

#include "UDPSocket.h"

class ClientSession
{
public:
    ClientSession(Server* pServer, Socket* pSocket, bool bUdpServer = false, bool bUdpClient = false);
    ~ClientSession();

public:
    Socket* getClientSocket() const
    {
        return clientSocket;
    }
    Server* getClientServer() const
    {
        return clientServer;
    }

    STATUS handleMessages();
    STATUS stateMachine(const char* recvMessage, int recvMsgLen);

    void setRemoteAddress(const sockaddr_in& clientAddr)
    {
        remoteAddress = clientAddr;
    }

    void setCurrentStatus(const STATUS status)
    {
        currentStatus = status;
    }

    bool isUdpClientSession()
    {
        return isUdpClient;
    }

private:
    Server* clientServer;
    Socket* clientSocket;
    bool isUdpServer;
    bool isUdpClient;

    sockaddr_in remoteAddress; // only for udp remote address
    STATUS currentStatus;
};

#endif // CLIENTSESSION_H
