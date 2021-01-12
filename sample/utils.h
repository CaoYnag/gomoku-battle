#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

typedef int SOCK;

 
const char* getlocalip(const char* eth_inf);

void getipportfromaddr(sockaddr_in* addr, char* ipstr, int ipstrlen, int* port);

enum MSG_TYPE {REG, HB, DATA, REDIRECT, REJECT};
typedef struct _MSG
{
    int type;
    char ip[256];
    int port;
    char data[2048];

    std::string info();
}MSG, *PMSG;

PMSG parseMSG(const char* buff);

const char* genData();