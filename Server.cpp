#include "Server.h"
#include "Constant.h"
#include "Utilities.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <sys/stat.h>

using namespace std;

#define SEND_RESPONSE(message)                                     \
    if(socketClient->isStreamSocket())                             \
        ret = socketClient->send(message.c_str(), message.size()); \
    else                                                           \
        ret = ((UDPSocket*)socketClient)->sendToClient(message.c_str(), message.size(), clientAddr);
#define CHECK_RET             \
    if(ret <= 0)              \
    {                         \
        status = ERROR_STATE; \
        break;                \
    }

bool operator<(struct sockaddr_in a, struct sockaddr_in b)
{
    if(a.sin_addr.s_addr < b.sin_addr.s_addr)
        return true;
    else if(a.sin_addr.s_addr == b.sin_addr.s_addr)
    {
        if(a.sin_port < b.sin_port)
            return true;
        else
            return false;
    }
    else
        return false;
}

Server::Server()
    : thread_pool(100)
{
    tcpServerSocket = NULL;
    udpServerSocket = NULL;
    serviceRunning = false;

    if(mkdir(FOLDER_NAME, 0777) == -1)
        FILE_LOG(logERROR) << "Error :  " << strerror(errno);
    else
        FILE_LOG(logINFO) << "Directory created";
}

Server::~Server()
{
    if(tcpServerSocket)
        delete tcpServerSocket;

    if(udpServerSocket)
        delete udpServerSocket;
    FILE_LOG(logINFO) << "End Server";

    assert(thread_pool.size() == 0);
}

bool Server::startService()
{
    if(serviceRunning)
        return false;

    serviceRunning = true;

    tcpServerSocket = new TCPSocket();
    tcpServerSocket->bindSocket();

    thread_pool.execute(std::bind(socketListeningHandler, this));

    udpServerSocket = new UDPSocket();
    udpServerSocket->bindSocket();

    thread_pool.execute(std::bind(udpConnectionHandler, this));

    return true;
}

bool Server::stopService()
{
    if(!serviceRunning)
        return false;

    serviceRunning = false;
    tcpServerSocket->closeSocket();
    udpServerSocket->closeSocket();

    delete tcpServerSocket;
    tcpServerSocket = NULL;

    delete udpServerSocket;
    udpServerSocket = NULL;

    assert(clientSessions.size() == 0);
    return true;
}

void* Server::socketListeningHandler(void* param)
{
    Server* service = static_cast<Server*>(param);
    if(service)
    {
        FILE_LOG(logINFO) << "start socketListeningHandler";

        TCPSocket* newClient = NULL;
        while(service->serviceRunning && (newClient = service->getTcpSocket()->acceptSocket()))
        {
            ClientSession* pSession = new ClientSession(service, newClient);

            pair<struct sockaddr_in, ClientSession*> pairClient = make_pair(newClient->getSocketInfo(), pSession);
            std::unique_lock<std::mutex> lk(service->mutex);
            auto ret = service->clientSessions.emplace(pairClient);
            lk.unlock();

            FILE_LOG(logINFO) << "create tcpConnectionHandler for " << newClient->getSocketInfoStr();
            service->thread_pool.execute(std::bind(tcpConnectionHandler, ret.first));
        }

        FILE_LOG(logINFO) << "end socketListeningHandler";
    }

    return 0;
}

/*
 * The connectionHandler will transmit tcp traffic
 * state machine flow will be generated  based on commands
 * each client/pair has own thread
 */
void* Server::tcpConnectionHandler(SocketMapItr param)
{
    ClientSession* pSession = param->second;
    Server* service = pSession->getClientServer();
    if(service)
    {
        FILE_LOG(logINFO) << "start tcpConnectionHandler";

        STATUS status = ERROR_STATE;
        Socket* pClientSocket = pSession->getClientSocket();
        while(service->serviceRunning && pSession != NULL && pClientSocket != NULL)
        {
            if(pClientSocket->isStreamSocket())
            {
                status = pSession->handleMessages();

                if(status == ERROR_STATE)
                {
                    FILE_LOG(logINFO) << "clean client info from server";

                    pClientSocket->closeSocket();
                    service->getTcpSocket()->removeClientSocket((TCPSocket*)pClientSocket);

                    std::unique_lock<std::mutex> lk(service->mutex);
                    service->clientSessions.erase(param);
                    lk.unlock();
                    delete pSession;
                    pSession = NULL;
                }
            }
        }
    }

    FILE_LOG(logINFO) << "end tcpConnectionHandler";
    return 0;
}

/*
 * The connectionHandler will transmit udp traffic
 * state machine flow will be generated  based on commands
 * demo is for a client/pair
 */
void* Server::udpConnectionHandler(void* param)
{
    Server* service = static_cast<Server*>(param);
    UDPSocket* pServerSocket = NULL;
    ClientSession* pSession = NULL;
    if(service)
    {
        FILE_LOG(logINFO) << "start udpConnectionHandler";

        pServerSocket = service->getUdpSocket();

        // for UDP, server is set to a client session as well
        pSession = new ClientSession(service, pServerSocket, true, false);
    }

    while(pServerSocket && pServerSocket->getConnected())
    {
        STATUS status = pSession->handleMessages();

        if(status == ERROR_STATE)
        {
            FILE_LOG(logINFO) << "udp server in error state";
            break;
        }
    }

    if(pSession)
    {
        std::unique_lock<std::mutex> lk(service->mutex);
        for(SocketMapItr itr = service->clientSessions.begin(); itr != service->clientSessions.end();)
        {
            if(itr->second->isUdpClientSession())
            {
                delete itr->second;
                service->clientSessions.erase(itr++);
            }
            else
            {
                ++itr;
            }
        }
        lk.unlock();

        delete pSession;
        pSession = NULL;
    }
    FILE_LOG(logINFO) << "end udpConnectionHandler";
    return 0;
}
