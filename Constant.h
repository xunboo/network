#ifndef CONSTANT_H
#define CONSTANT_H

enum COMMANDS { hello,okey,test,ack,file,size,offset,transmit,done,error };

static const char *g_Commands[]={ "@HELLO@", "@OKAY@", "@TEST@", "@ACK@", "@FILE@", "@SIZE@", "@OFFSET@","@TRANSMIT@","@DONE@", "@ERROR@" };
static const char *g_Full_Commands[]={ "@HELLO@", "@OKAY@", "@TEST@", "@ACK@", "@FILE@%s", "@SIZE@%s", "@OFFSET@%s","@TRANSMIT@%s","@DONE@","@ERROR@" };

// use state machine to save current status
enum STATUS { IDLE_STATE,READY_STATE,TEST_STATE,FILE_STATE,TRANSMIT_STATE,DONE_STATE,ERROR_STATE };

static const char *g_Status[]={ "IDLE_STATE","READY_STATE","TEST_STATE","FILE_STATE","TRANSMIT_STATE","DONE_STATE","ERROR_STATE" };

#define MAX_RECV_PACKET 1400        // it is a safty value below MTU for packet, client is using it for sending each block
                                    // local test can set a large value and save more time , 
                                    // but due to MTU and ethernet limitation, the packet size above MTU will be fragmented over network
#define MAX_RECV_TIMEOUT 10
#define MAX_LISTEN_COUNT SOMAXCONN   
#define TCP_PORT 12345
#define UDP_PORT 12345


#define FOLDER_NAME     "receive"

#endif // CONSTANT_H