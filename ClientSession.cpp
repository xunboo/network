#include "ClientSession.h"
#include "Utilities.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Server.h"

using namespace std;

#define SEND_RESPONSE(message)                                                   \
    if(clientSocket->isStreamSocket())                                           \
        ret = ((TCPSocket*)clientSocket)->send(message.c_str(), message.size()); \
    else                                                                         \
        ret = ((UDPSocket*)clientSocket)->sendToClient(message.c_str(), message.size(), clientAddr);
#define CHECK_RET             \
    if(ret <= 0)              \
    {                         \
        status = ERROR_STATE; \
        break;                \
    }

ClientSession::ClientSession(Server* pServer, Socket* pSocket, bool bUdpServer, bool bUdpClient)
    : clientServer(pServer)
    , clientSocket(pSocket)
{
    isUdpServer = bUdpServer;
    isUdpClient = bUdpClient;
    currentStatus = IDLE_STATE;
}

ClientSession::~ClientSession()
{
}

STATUS ClientSession::handleMessages()
{
    STATUS status = IDLE_STATE;
    int ret = 1;
    // for UDP, clientSocket is same as serverSocket
    while(clientSocket && clientSocket->getConnected() && ret > 0)
    {
        char buffer[MAX_RECV_PACKET] = { 0 };

        int retWait = 1;
        if(isUdpServer)
        { // only udp server need wait and recv
            retWait = clientSocket->wait();
        }

        if(retWait == 1)
        {
            if(clientSocket->isStreamSocket())
            {
                ret = ((TCPSocket*)clientSocket)->receive(buffer, MAX_RECV_PACKET);
                CHECK_RET;

                if(ret > 0 && ret < 10)
                {
                    FILE_LOG(logINFO) << "TCP server recv msg: " << buffer;
                }

                status = stateMachine(buffer, ret);
                currentStatus = status;
            }
            else
            {
                if(isUdpServer)
                {
                    sockaddr_in clientAddr;
                    ret = ((UDPSocket*)clientSocket)->recvFromClient(buffer, MAX_RECV_PACKET, clientAddr);
                    CHECK_RET;

                    if(ret > 0 && ret < 10)
                    {
                        FILE_LOG(logINFO) << "UDP server recv msg: " << buffer;
                    }

                    SocketMapItr socketMapItr = clientServer->getClientSessions().find(clientAddr);
                    ClientSession* pClientSession = NULL;
                    if(socketMapItr != clientServer->getClientSessions().end())
                    {
                        pClientSession = socketMapItr->second;
                    }
                    else
                    {
                        pClientSession = new ClientSession(clientServer, clientSocket, false, true);
                        pClientSession->setRemoteAddress(clientAddr);

                        pair<struct sockaddr_in, ClientSession*> pairClient = make_pair(clientAddr, pClientSession);
                        std::unique_lock<std::mutex> lk(clientServer->mutex);
                        clientServer->getClientSessions().emplace(pairClient);
                        lk.unlock();

                        FILE_LOG(logINFO) << "create a new UDP client session from remote "
                                           << clientSocket->getRemoteSocketStr(clientAddr);
                    }

                    status = pClientSession->stateMachine(buffer, ret);
                    pClientSession->setCurrentStatus(status);
                }
            }
        }
        else if(retWait < 0) // udp socket is broken
        {
            ret = -1;
            CHECK_RET;
        }
    }

    return status;
}

STATUS ClientSession::stateMachine(const char* recvMessage, int recvMsgLen)
{
    string message;
    STATUS status = currentStatus;
    static string pathFile;
    sockaddr_in clientAddr = remoteAddress;
    int ret = 0;
    const char* buffer = recvMessage;

    int pre_status = status;
    switch(status)
    {
    case IDLE_STATE:
        if(StrNCmp(buffer, g_Commands[hello]) == 0)
        {
            message = g_Commands[okey];
            SEND_RESPONSE(message)
            CHECK_RET;

            status = READY_STATE;
        }
        break;

    case READY_STATE:
        if(StrNCmp(buffer, g_Commands[test]) == 0)
        {
            message = g_Commands[ack];
            SEND_RESPONSE(message)
            CHECK_RET;

            status = TEST_STATE;
        }
        else if(StrNCmp(buffer, g_Commands[file]) == 0)
        {
            char filename[256];
            sscanf(buffer, g_Full_Commands[file], filename);
            FILE_LOG(logINFO) << "get filename: " << filename;
            string path(FOLDER_NAME);
            pathFile = path + "/" + filename;

            fstream ffile(pathFile, ios::binary | ios::app | fstream::out);
            if(ffile.good()) // create
            {
                FILE_LOG(logINFO) << "file exist: " << pathFile;
                ffile.close();
            }
            else
            {
                ffile.close();
                FILE_LOG(logERROR) << "open " << pathFile << "file failed!";

                message = g_Commands[error];
                SEND_RESPONSE(message)
                CHECK_RET;

                // status = IDLE_STATE;
                break;
            }

            message = g_Commands[ack];
            SEND_RESPONSE(message)
            CHECK_RET;

            status = FILE_STATE;
        }
        else if(StrNCmp(buffer, g_Commands[hello]) == 0)
        {
            status = IDLE_STATE;

            message = g_Commands[ack];
            SEND_RESPONSE(message)
            CHECK_RET;
        }

        break;

    case FILE_STATE:
        if(StrNCmp(buffer, g_Commands[size]) == 0)
        {
            unsigned int filesize = 0;
            char fileinfo[MAX_RECV_PACKET];
            sscanf(buffer, g_Full_Commands[size], fileinfo);
            filesize = atoi(fileinfo);
            FILE_LOG(logINFO) << "get " << pathFile << "file size: " << filesize;

            ifstream ifile(pathFile, std::ios_base::binary | std::ios_base::ate);
            if(ifile.fail())
            {
                ifile.close();
                FILE_LOG(logERROR) << "open " << pathFile << "file failed!";

                message = g_Commands[error];
                SEND_RESPONSE(message)
                CHECK_RET;

                // status = IDLE_STATE;
                break;
            }
            unsigned int filepos = ifile.tellg();
            stringstream ss;
            ss << filepos;
            snprintf(fileinfo, MAX_RECV_PACKET - 1, g_Full_Commands[offset], ss.str().c_str());
            FILE_LOG(logINFO) << "get file pos: " << filepos;
            ifile.close();

            message = fileinfo;
            SEND_RESPONSE(message)
            CHECK_RET;

            status = TRANSMIT_STATE;
        }
        else if(StrNCmp(buffer, g_Commands[hello]) == 0)
        {
            status = IDLE_STATE;

            message = g_Commands[ack];
            SEND_RESPONSE(message)
            CHECK_RET;
        }
        else if(StrNCmp(buffer, g_Commands[done]) == 0)
        {
            status = IDLE_STATE;
            FILE_LOG(logINFO) << "success getting whole file ";
        }

        break;

    case TRANSMIT_STATE:
        if(StrNCmp(buffer, g_Commands[transmit]) == 0)
        {
            unsigned int newpos = 0;
            char fileinfo[256];
            sscanf(buffer, g_Full_Commands[transmit], fileinfo);
            newpos = atoi(fileinfo);
            // FILE_LOG(logINFO) << "get new file pos: "<<newpos;

            message = g_Commands[ack];
            SEND_RESPONSE(message)
            CHECK_RET;
        }
        else if(StrNCmp(buffer, g_Commands[done]) == 0)
        {
            status = IDLE_STATE;
            FILE_LOG(logINFO) << "success getting whole file";
        }
        else if(StrNCmp(buffer, g_Commands[hello]) == 0)
        {
            status = IDLE_STATE;

            message = g_Commands[ack];
            SEND_RESPONSE(message)
            CHECK_RET;
        }
        else
        { // receive new block
            fstream ffile(pathFile, ios::binary | ios::app | fstream::out);
            if(ffile.good())
            {
                ffile.write(buffer, recvMsgLen);
                //   FILE_LOG(logINFO) << "get new file block size: "<<recvMsgLen;
                ffile.close();

                message = g_Commands[ack];
                SEND_RESPONSE(message)
                CHECK_RET;
            }
        }
        break;

    case TEST_STATE:
        status = IDLE_STATE;
        break;

    default:
        break;
    }

    if(pre_status != status)
    {
        FILE_LOG(logINFO) << "change status from " << g_Status[pre_status] << " to " << g_Status[status];
    }

    return status;
}
