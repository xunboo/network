#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "Utilities.h"
#include "UnitTest.h"
#include "Server.h"
#include "Client.h"

using namespace std;


void ignore_sigpipe(void)
{
    struct sigaction sact;
    int r;
    memset(&sact, 0, sizeof(sact));
    sact.sa_handler = SIG_IGN;
    sact.sa_flags = SA_RESTART;
    r = ::sigaction(SIGPIPE, &sact, NULL);
}

int main(int argc, char **argv)
{
    //ignore SIGPIPE, add it because "Terminated due to signal 13" was received during socket testing
    ignore_sigpipe();
    
	cout<<"This is a file transfer demo"<<endl;
    cout<<"----------------------------"<<endl;
    if(argc < 2){
        cout<<"-Usage: ./FileTransfer [-s|-c] [-t|-u] [optional: -f filename -h host -b bandwidth]" <<endl;
        cout<<"-s: server mode; -c: client mode; -t: use TCP as protocol for client; -u: use UDP as protocol for client"<<endl;
        cout<<"When it is server mode, both tcp and udp are enabled" << endl;
        cout<<"If -c is used, -f (filename) and -h (server host address) are needed" << endl;
        cout<<"If -f or -h is empty, default filename is 1080p.mp4 and host is localhost" << endl;
        cout<<"-b to set upload bandwidth for client, min value is 2 (2KB/s, which is bigger than MTU)" << endl;
        cout<<"The service listening port is 12345, it may need be opened in the network" << endl;
        cout<<"Example: ./FileTransfer -c -t -f 1080p.mp4 -h 127.0.0.1" << endl;
        cout<<"Tips: input q to quit; input t/r to run UnitTests" << endl;
        cout<<"-----------------------------------------------------------------------------"<<endl;
    }
    
    int cur_argv = 1;
    bool bServer = false;
    bool bClient = false;
    bool bTcp = false;
    bool bUdp = false;
    string filename("1080p.mp4");
    string hostname("127.0.0.1");
    int bandwidth = -1;
    while(cur_argv < argc){
        if(strcmp(argv[cur_argv], "-s") == 0 && !bClient){
            // server mode
            bServer = true;
            bTcp = true;
            bUdp = true;
        }
        else if(strcmp(argv[cur_argv], "-c") == 0 && !bServer){
            // client mode
            bClient = true;   
        }
        
        if(strcmp(argv[cur_argv], "-t") == 0 && !bUdp){
            // server mode
            bTcp = true;
        }
        else if(strcmp(argv[cur_argv], "-u") == 0 && !bTcp){
            // client mode
            bUdp = true;   
        }
        
        if(bClient){
            if(strcmp(argv[cur_argv], "-f") == 0 && cur_argv + 1 < argc){
                filename = argv[cur_argv + 1];
            }  
            else if(strcmp(argv[cur_argv], "-h") == 0 && cur_argv + 1 < argc){
                hostname = argv[cur_argv + 1];
            }    
            else if(strcmp(argv[cur_argv], "-b") == 0 && cur_argv + 1 < argc){
                bandwidth = atoi(argv[cur_argv + 1]);
                if(bandwidth<2) bandwidth = 2;
            }      
        }
        
        cur_argv++;
    }
    
    if(filename.length() > 200 || hostname.length() > 100){
        FILE_LOG(logERROR) << "error parameter";
        return 0;
    }
    
    ifstream f(filename.c_str());
    if(f.good() == false){
        FILE_LOG(logERROR) << "upload file does not exist";
        return 0;        
    }
    
    if(!bServer && !bClient){
        bServer = true;
        bTcp = true;
        bUdp = true;
    }
    
    if(!bTcp && !bUdp){
        bTcp = true;
    }
    
    Server* pServer = NULL;
    Client* pClient = NULL;
    try{
        if(bServer){
            FILE_LOG(logINFO)<<"Start Server...";
                
            pServer = new Server();
            pServer->startService();
        }
        else if(bClient){
            if(bTcp){
                FILE_LOG(logINFO)<<"Start Client with TCP mode to host "<< hostname << ", sending file " << filename;
                
                pClient = new Client(hostname, filename);
                pClient->setBandWidthLimit(bandwidth);
                pClient->startClient(true);
                TIMEPOINT start = NOW;
                TIMEPOINT end;
                if(pClient->sendFile() == 1){
                    end = NOW;
                }
                else{
                    //resend
                    
                }
                
                chrono::duration<double> time_span = chrono::duration_cast< chrono::duration<double> >(end - start);
                FILE_LOG(logINFO) << "Send file done. It took " << time_span.count() << " seconds.";
            }
            else{
                FILE_LOG(logINFO)<<"Start Client with UDP mode to host "<< hostname << ", sending file " << filename;
                
                pClient = new Client(hostname, filename);
                pClient->setBandWidthLimit(bandwidth);
                pClient->startClient(false);
                TIMEPOINT start = NOW;
                TIMEPOINT end;
                if(pClient->sendFile() == 1){
                    end = NOW;
                }
                else{
                    //resend
                    
                }
                
                chrono::duration<double> time_span = chrono::duration_cast< chrono::duration<double> >(end - start);
                FILE_LOG(logINFO) << "Send file done. It took " << time_span.count() << " seconds.";
            }      
        }
    }
    catch(std::runtime_error& e){
        FILE_LOG(logERROR) << e.what();
    }
    
    sleep(2);
    printf("cmd: ");
    int c;
    do {
        c=getchar();
        
        if(bServer){
            if(c=='t'){
                UnitTest::RunAllServerTests();
            }
            if(c=='r'){
                UnitTest::RunAllClientTests();
            }
        }
            
    } while (c != 'q');
    
    if(pServer){
        FILE_LOG(logINFO)<<"Stop Server...";
        pServer->stopService();
        delete pServer;
    }
    
    if(pClient){
        FILE_LOG(logINFO)<<"Stop Client...";
        pClient->stopClient();
        delete pClient;
    }
	return 0;
}
