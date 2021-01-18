#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <memory>
#include <unistd.h>
using namespace std;

typedef int SOCK; 
 
const char* getlocalip(const char* eth_inf);

void getipportfromaddr(sockaddr_in* addr, char* ipstr, int ipstrlen, int* port);


class TcpSock
{
protected:
    SOCK _sock;
    sockaddr_in _addr;
    int _port;
    string _ip;
public:
    TcpSock();
    TcpSock(int port);
    TcpSock(const string& ip, int port);
    TcpSock(SOCK sock, const sockaddr_in& addr);
    virtual ~TcpSock();

    /* resource intfs. */
    int init();
    void close();
    inline string ip() {return _ip;}
    inline int port(){return _port;}
    inline SOCK sock(){return _sock;}

    /* communicate intfs */
    int listen(int conn = SOMAXCONN);
    int connect(const string& ip, int port);
    shared_ptr<TcpSock> accept();
    int send(const string& data);
    string recv(int buff_len = 4096);
};