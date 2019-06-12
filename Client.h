#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Socket.h"
#include "Constant.h"

class Client
{
public:
    Client(const std::string& hostname, const std::string& filename);
    ~Client();

public:
    bool startClient(bool bStream);
    bool stopClient();

    int sendFile();
    void setBandWidthLimit(int nLimit)
    {
        bandWidthLimit = nLimit;
    };
    float getUploadBandWidth()
    {
        return uploadBandWidth;
    }

private:
    int requestResponse(const char* command, const char* expected, const char* param = NULL);

private:
    bool streamSocket;
    bool clientRunning;
    int bandWidthLimit;    // unit is KB/s
    float uploadBandWidth; // current avearge bandwidth, unit is KB/s

    Socket* serverSocket;

    std::string hostname;
    std::string filename;
};

#endif // CLIENT_H
