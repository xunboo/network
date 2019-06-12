#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <map>

#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Constant.h"
#include "ClientSession.h"
#include "Utilities.h"
#include "ThreadPool.h"

typedef std::map<struct sockaddr_in, ClientSession*> SocketMap;
typedef std::map<struct sockaddr_in, ClientSession*>::const_iterator SocketMapItr;

class Server
{
public:
    Server();
    ~Server();

public:
    bool startService();
    bool stopService();

    inline TCPSocket* getTcpSocket()
    {
        return tcpServerSocket;
    }
    inline UDPSocket* getUdpSocket()
    {
        return udpServerSocket;
    }

    SocketMap& getClientSessions()
    {
        return clientSessions;
    }

private:
    static void* socketListeningHandler(void* param);
    static void* tcpConnectionHandler(SocketMapItr param);
    static void* udpConnectionHandler(void* param);

    static STATUS stateMachine(Socket* socketClient);

private:
    TCPSocket* tcpServerSocket;
    UDPSocket* udpServerSocket;

    // <clientAddr with its session>
    // For TCP, each tcp connection from remote has own thread and clientSession
    // For UDP, each udp connection from remote has a clientSession.
    // TODO: clientSessions should clean UDP clientSession if there is no traffic from remote after a period time.
    SocketMap clientSessions;

    volatile bool serviceRunning;
    fixed_thread_pool thread_pool;

public:
    std::mutex mutex;
};

#endif // SERVER_H
