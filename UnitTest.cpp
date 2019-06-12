#include <stdio.h>
#include "UnitTest.h"
#include "Constant.h"
#include "Utilities.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <array>

#include "Server.h"
#include "Client.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

using namespace std;

// add it to run hash command, need shasum in system
static std::string exec(const char* command)
{
    std::string result;
    string ret;
    FILE* pipe = popen(command, "r");
    if(pipe)
    {
        array<char, 128> buffer;

        if(fgets(buffer.data(), 128, pipe) != NULL)
        {
            result += buffer.data();
        }

        stringstream ss(result);
        getline(ss, ret, ' ');

        pclose(pipe);
    }

    return ret;
}

UnitTest::UnitTest()
{
}

UnitTest::~UnitTest()
{
}

void UnitTest::RunAllServerTests()
{
    FILE_LOG(logINFO) << "Start RunAllServerTests...";
    // local socket test to server socket
    TestTcpSocket(0);
    TestUdpSocket(0);

    TestTcpMultiSockets();
    TestUdpMultiSockets();

    // simulate msg-done drop for udp, verify recv timeout work
    TestUdpSocket(1);
    sleep(MAX_RECV_TIMEOUT + 1);
    TestUdpSocket(0);

    FILE_LOG(logINFO) << "Finish RunAllServerTests...";
}

void UnitTest::RunAllClientTests()
{
    FILE_LOG(logINFO) << "Start RunAllClientTests...";

    // local client test to server
    TestClientTcpConnect();
    TestClientUdpConnect();

    sleep(2);

    TestClientTcpSendFile();
    TestClientUdpSendFile();

    sleep(2);
    TestClientTcpSendFileWithReconn();

    sleep(2);

    TestClientTcpSendFileWithBandLimit();
    TestClientUdpSendFileWithBandLimit();

    FILE_LOG(logINFO) << "Finish RunAllClientTests...";
}

void* UnitTest::tcpClientThread(void* param)
{
    FILE_LOG(logINFO) << "tcpClientThread:" << *(int*)param;

    UnitTest::TestTcpSocket(2);
    free(param);
    return 0;
}

void UnitTest::TestTcpMultiSockets()
{
    FILE_LOG(logINFO) << "TestTcpMultiSockets";

    pthread_t thread_id[10];
    for(int i = 0; i < 10; i++)
    {
        int* arg = (int*)malloc(sizeof(*arg));
        *arg = i;
        assert(pthread_create(&thread_id[i], NULL, tcpClientThread, arg) == 0);

        //    sleep(1);
    }

    void* ret;
    for(int i = 0; i < 10; i++)
    {
        assert(pthread_join(thread_id[i], &ret) == 0);
    }
}

void* UnitTest::udpClientThread(void* param)
{
    FILE_LOG(logINFO) << "udpClientThread:" << *(int*)param;

    UnitTest::TestUdpSocket(2);
    free(param);
    return 0;
}

void UnitTest::TestUdpMultiSockets()
{
    FILE_LOG(logINFO) << "TestTcpMultiSockets";

    pthread_t thread_id[10];
    for(int i = 0; i < 10; i++)
    {
        int* arg = (int*)malloc(sizeof(*arg));
        *arg = i;
        assert(pthread_create(&thread_id[i], NULL, udpClientThread, arg) == 0);

        //    sleep(1);
    }

    void* ret;
    for(int i = 0; i < 10; i++)
    {
        assert(pthread_join(thread_id[i], &ret) == 0);
    }
}

// setup a tcp socket to remote
void UnitTest::TestTcpSocket(int option)
{
    FILE_LOG(logINFO) << "TestTcpSocket";

    TCPSocket socket;
    char str[20] = { 0 };

    socket.connect("127.0.0.1");

    if(option == 2)
    {
        sleep(1);
    }

    socket.send(g_Commands[hello], strlen(g_Commands[hello]));

    socket.receive(str, 20);
    assert(strcmp(str, g_Commands[okey]) == 0);

    socket.send(g_Commands[test], strlen(g_Commands[test]));
    memset(str, 0, 20);
    socket.receive(str, 20);
    assert(strcmp(str, g_Commands[ack]) == 0);

    if(option != 1)
    {
        socket.send(g_Commands[done], strlen(g_Commands[done]));
    }
}

// setup a udp socket to remote
void UnitTest::TestUdpSocket(int option)
{
    FILE_LOG(logINFO) << "TestUdpSocket";

    UDPSocket socket;
    char str[20] = { 0 };

    if(option == 2)
    {
        sleep(1);
    }

    socket.connect("127.0.0.1");

    socket.send(g_Commands[hello], strlen(g_Commands[hello]));

    socket.receive(str, 20);
    assert(strcmp(str, g_Commands[okey]) == 0);

    socket.send(g_Commands[test], strlen(g_Commands[test]));
    memset(str, 0, 20);
    socket.receive(str, 20);
    assert(strcmp(str, g_Commands[ack]) == 0);

    if(option != 1)
    {
        socket.send(g_Commands[done], strlen(g_Commands[done]));
    }
}

// simulate send a file by TCP
void UnitTest::TestClientTcpConnect()
{
    Client* pClient = new Client("127.0.0.1", "");
    assert(pClient->startClient(true));

    assert(pClient->sendFile() == 1);

    assert(pClient->stopClient());

    delete pClient;
}

// simulate send a file by UDP
void UnitTest::TestClientUdpConnect()
{
    Client* pClient = new Client("127.0.0.1", "");
    assert(pClient->startClient(false));

    assert(pClient->sendFile() == 1);

    assert(pClient->stopClient());

    delete pClient;
}

// send a real file over TCP for testing
void UnitTest::TestClientTcpSendFile()
{
    string hash1 = exec("shasum 1080p.mp4");
    FILE_LOG(logINFO) << hash1;

    remove("receive/1080p.mp4");
    Client* pClient = new Client("127.0.0.1", "1080p.mp4");
    assert(pClient->startClient(true));

    TIMEPOINT start = NOW;
    assert(pClient->sendFile() == 1);
    TIMEPOINT end = NOW;

    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(end - start);
    FILE_LOG(logINFO) << "send file took " << time_span.count() << " seconds.";

    assert(pClient->stopClient());

    delete pClient;

    string hash2 = exec("shasum receive/1080p.mp4");
    assert(hash1 == hash2);
}

// send a real file over UDP for testing
void UnitTest::TestClientUdpSendFile()
{
    string hash1 = exec("shasum 1080p.mp4");
    FILE_LOG(logINFO) << hash1;

    remove("receive/1080p.mp4");
    Client* pClient = new Client("127.0.0.1", "1080p.mp4");
    assert(pClient->startClient(false));

    TIMEPOINT start = NOW;
    assert(pClient->sendFile() == 1);
    TIMEPOINT end = NOW;

    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(end - start);
    FILE_LOG(logINFO) << "send file took " << time_span.count() << " seconds.";

    assert(pClient->stopClient());

    delete pClient;

    string hash2 = exec("shasum receive/1080p.mp4");
    assert(hash1 == hash2);
}

volatile bool bClientDrop = false;
void* tcpClientDrop(void* param)
{
    sleep(1); // drop connection after 1s
    FILE_LOG(logINFO) << "drop client connection";
    Client* pClient = (Client*)param;
    pClient->stopClient();

    bClientDrop = true;
    return 0;
}

void UnitTest::TestClientTcpSendFileWithReconn()
{
    string hash1 = exec("shasum 1080p.mp4");
    FILE_LOG(logINFO) << hash1;

    remove("receive/1080p.mp4");
    Client* pClient = new Client("127.0.0.1", "1080p.mp4");
    assert(pClient->startClient(true));

    pthread_t thread_id;
    assert(pthread_create(&thread_id, NULL, tcpClientDrop, pClient) >= 0);

    TIMEPOINT start = NOW;
    if(pClient->sendFile() == 0)
    {
        while(bClientDrop == false)
            ;

        sleep(5);
        FILE_LOG(logINFO) << "start new connection after drop";
        pClient->startClient(true);
        assert(pClient->sendFile() == 1);
    }
    TIMEPOINT end = NOW;

    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(end - start);
    FILE_LOG(logINFO) << "send file took " << time_span.count() << " seconds.";

    assert(pClient->stopClient());

    delete pClient;

    string hash2 = exec("shasum receive/1080p.mp4");
    assert(hash1 == hash2);
}

// send a real file over TCP for testing with bandwidth limit
void UnitTest::TestClientTcpSendFileWithBandLimit()
{
    string hash1 = exec("shasum 1080p.mp4");
    FILE_LOG(logINFO) << hash1;

    remove("receive/1080p.mp4");
    Client* pClient = new Client("127.0.0.1", "1080p.mp4");
    pClient->setBandWidthLimit(2000); // max 2000KB/S
    assert(pClient->startClient(true));

    TIMEPOINT start = NOW;
    assert(pClient->sendFile() == 1);
    TIMEPOINT end = NOW;

    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(end - start);
    FILE_LOG(logINFO) << "send file took " << time_span.count() << " seconds.";

    float rate = pClient->getUploadBandWidth();
    assert(rate > 1980.0f && rate < 2020.0f);

    assert(pClient->stopClient());

    delete pClient;

    string hash2 = exec("shasum receive/1080p.mp4");
    assert(hash1 == hash2);
}

// send a real file over UDP for testing with bandwidth limit
void UnitTest::TestClientUdpSendFileWithBandLimit()
{
    string hash1 = exec("shasum 1080p.mp4");
    FILE_LOG(logINFO) << hash1;

    remove("receive/1080p.mp4");
    Client* pClient = new Client("127.0.0.1", "1080p.mp4");
    pClient->setBandWidthLimit(2000); // max 2000KB/S
    assert(pClient->startClient(false));

    TIMEPOINT start = NOW;
    assert(pClient->sendFile() == 1);
    TIMEPOINT end = NOW;

    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(end - start);
    FILE_LOG(logINFO) << "send file took " << time_span.count() << " seconds.";

    float rate = pClient->getUploadBandWidth();
    assert(rate > 1980.0f && rate < 2020.0f);

    assert(pClient->stopClient());

    delete pClient;

    string hash2 = exec("shasum receive/1080p.mp4");
    assert(hash1 == hash2);
}