#ifndef UNITTEST_H
#define UNITTEST_H

class UnitTest
{
public:
    UnitTest();
    ~UnitTest();

public:
    static void RunAllServerTests();
    static void RunAllClientTests();

private:
    static void* tcpClientThread(void* param);
    static void* udpClientThread(void* param);

    // AllTests
private:
    static void TestTcpSocket(int option);
    static void TestUdpSocket(int option);
    static void TestTcpMultiSockets();
    static void TestUdpMultiSockets();

    static void TestClientTcpConnect();
    static void TestClientUdpConnect();

    static void TestClientTcpSendFile();
    static void TestClientUdpSendFile();
    static void TestClientTcpSendFileWithReconn();
    static void TestClientTcpSendFileWithBandLimit();
    static void TestClientUdpSendFileWithBandLimit();
};

#endif // UNITTEST_H
