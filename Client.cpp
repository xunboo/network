#include "Client.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Utilities.h"

using namespace std;

#define BUF_SIZE 20
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

Client::Client(const string& hostname, const string& filename)
{
    clientRunning = false;
    serverSocket = NULL;
    this->hostname.assign(hostname);
    this->filename.assign(filename);
    bandWidthLimit = -1;
    uploadBandWidth = 0.0f;
}

Client::~Client()
{
    if(serverSocket)
        delete serverSocket;

    FILE_LOG(logINFO) << "End Client";
}

bool Client::startClient(bool bStream)
{
    if(clientRunning)
        return false;

    clientRunning = true;
    streamSocket = bStream;

    if(streamSocket)
    {
        serverSocket = new TCPSocket();
        serverSocket->connect(hostname.c_str(), TCP_PORT);
    }
    else
    {
        serverSocket = new UDPSocket();
        serverSocket->connect(hostname.c_str(), UDP_PORT);
    }

    return true;
}

bool Client::stopClient()
{
    if(!clientRunning)
        return false;

    serverSocket->closeSocket();

    clientRunning = false;
    return true;
}

int Client::sendFile()
{
    if(filename.length() == 0) // test mode, simulate a file transmit
    {
        if(requestResponse(g_Full_Commands[hello], g_Commands[okey]) == 0)
        {
            if(requestResponse(g_Full_Commands[file], g_Commands[ack], "testfile") == 0) // filename
            {
                int pos = requestResponse(g_Full_Commands[size], g_Commands[offset], "1024");
                // printf("get pos %d\n", pos);
                if(pos >= 0) // current file last postion
                {
                    if(requestResponse(g_Full_Commands[transmit], g_Commands[ack], "100") ==
                        0) // transmit file block from last postion
                    {
                        if(requestResponse(g_Full_Commands[done], NULL) ==
                            0) // send done when all the file is transmitted
                        {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(requestResponse(g_Full_Commands[hello], g_Commands[okey]) == 0)
        {
            if(requestResponse(g_Full_Commands[file], g_Commands[ack], filename.c_str()) == 0) // filename
            {
                struct stat statbuf;
                if(stat(filename.c_str(), &statbuf) == -1)
                {
                    return -1;
                }
                unsigned int filesize = statbuf.st_size;
                stringstream ss;
                ss << filesize;
                FILE_LOG(logINFO) << "filesize: " << filesize;
                if(filesize == 0)
                {
                    requestResponse(g_Full_Commands[done], NULL); // don't send empty file
                    return 1;
                }

                int pos = requestResponse(
                    g_Full_Commands[size], g_Commands[offset], ss.str().c_str()); // get current file size in server
                if(pos >= 0 && pos < filesize)                                    // check filesize
                {
                    ifstream ifile(filename, std::ios_base::binary);
                    ifile.seekg(pos, ios::beg);
                    FILE_LOG(logINFO) << "sending file now..." << pos;
                    bool failed = false;
                    uploadBandWidth = 0.0f;
                    TIMEPOINT quota_start = NOW;
                    int total_sent_in_a_second = 0;
                    int total_sent_in_full = 0;
                    TIMEPOINT send_start = NOW;

                    while(pos < filesize)
                    {
                        int block = MIN(filesize - pos, MAX_RECV_PACKET);
                        // printf("block %d--", block);
                        stringstream ss2;
                        ss2 << pos;
                        if(requestResponse(g_Full_Commands[transmit], g_Commands[ack], ss2.str().c_str()) == 0)
                        {
                            // transmit block from current postion
                            // FILE_LOG(logINFO) << pos << ",";
                            char* request = new char[block];
                            ifile.read(request, block);

                            int retry = 0; // retry is used for UDP packet loss case
                            do
                            {
                                // TIMEPOINT start = NOW;
                                serverSocket->send(request, block);
                                total_sent_in_a_second += block;
                                total_sent_in_full += block;

                                char getack[6] = { 0 };
                                int result = serverSocket->receive(getack, strlen(g_Commands[ack]));

                                if(bandWidthLimit > 2)
                                {
                                    chrono::duration<double, std::milli> quota_span =
                                        chrono::duration_cast<chrono::duration<double, std::milli> >(NOW - quota_start);
                                    if(quota_span.count() >= 1000)
                                    {
                                        total_sent_in_a_second = 0;
                                        quota_start = NOW;
                                    }
                                    else
                                    {
                                        if(total_sent_in_a_second >= bandWidthLimit * 1024)
                                        { // bandwidth limit
                                            int wait = 1000 - quota_span.count();
                                            std::this_thread::sleep_for(chrono::milliseconds(wait));
                                        }
                                    }
                                }

                                if(result > 0 && StrNCmp(getack, g_Commands[ack]) == 0)
                                    break;
                                retry++;
                            } while(retry < 3); // retry 3 times

                            delete[] request;
                            if(retry == 3)
                            {
                                FILE_LOG(logERROR) << "socker error for sending file!";
                                failed = true;
                                break;
                            }
                            pos += block;
                        }
                        else
                        { // socket drop
                            failed = true;
                            FILE_LOG(logERROR) << "connection drop, failed to send whole file";
                            break;
                        }
                    }

                    ifile.close();
                    TIMEPOINT send_end = NOW;
                    chrono::duration<double> time_span =
                        chrono::duration_cast<chrono::duration<double> >(send_end - send_start);
                    uploadBandWidth = total_sent_in_full / 1024 / time_span.count();
                    FILE_LOG(logINFO) << std::fixed << std::setprecision(3) << "sending file done... upload rate is "
                                       << uploadBandWidth << "KB/s";
                    if(failed)
                    {
                        return 0;
                    }

                    // FILE_LOG(logINFO);

                    requestResponse(g_Full_Commands[done], NULL); // send done when all the file is transmitted
                    return 1;
                }
                else if(pos == filesize)
                {
                    // already send whole file
                    requestResponse(g_Full_Commands[done], NULL);
                    return 1;
                }
            }
        }
    }

    return 0;
}

int Client::requestResponse(const char* command, const char* expected, const char* param)
{
    int nCount = 0;
    char* request = NULL;
    int output = 0;

    if(param == NULL)
    {
        request = (char*)command;
    }
    else
    {
        request = new char[MAX_RECV_PACKET];
        snprintf(request, MAX_RECV_PACKET - 1, command, param);
        // printf("request: %s\n", request);
    }

    while(nCount < 3)
    {
        serverSocket->send(request, strlen(request) + 1);
        if(expected == NULL)
            return 0;

        char response[BUF_SIZE] = { 0 };
        int recv_size = serverSocket->receive(response, BUF_SIZE);
        // printf("response: %s\n", response);
        if(recv_size >= 0 && StrNCmp(response, expected) == 0)
        {
            int exp_size = strlen(expected);
            if(recv_size > exp_size)
            {
                output = atoi(response + exp_size);
            }

            if(param)
                delete[] request;
            return output;
        }
        else
        {
            nCount++;
            //printf("retry %d\n", nCount);
        }
    }

    if(param)
        delete[] request;
    return -1;
}