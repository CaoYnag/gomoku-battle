#include "net_utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <spdlog/spdlog.h>
using namespace std;
 
const char* getlocalip(const char *eth_inf)  
{  
    int sd;  
    sockaddr_in sin;  
    ifreq ifr;
    char* ip = new char[1024];
  
    sd = socket(AF_INET, SOCK_DGRAM, 0);  
    if (-1 == sd)  
    {  
        printf("socket error: %s\n", strerror(errno));  
        return nullptr;        
    }  
  
    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);  
    ifr.ifr_name[IFNAMSIZ - 1] = 0;  
      
    // if error: No such device  
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)  
    {  
        close(sd);
        return nullptr;        
    }  
  
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));  
    snprintf(ip, 16, "%s", inet_ntoa(sin.sin_addr));  
  
    close(sd);  
    return ip;  
}

void getipportfromaddr(struct sockaddr_in * addr ,char * ipstr,int ipstrlen,int* port)
{
    *port = ntohs(addr->sin_port);
    in_addr in  = addr->sin_addr;
  
    inet_ntop(AF_INET,&in, ipstr, ipstrlen);
}

TcpSock::TcpSock() : _sock(0), _port(0), _ip("")
{}
TcpSock::TcpSock(int port) : _sock(0), _port(port), _ip("")
{}
TcpSock::TcpSock(const string& ip, int port) : _sock(0), _port(port), _ip(ip)
{}

TcpSock::TcpSock(SOCK sock, const sockaddr_in& addr) : _sock(sock)
{
    memcpy(&_addr, &addr, sizeof(addr));
    _port = ntohs(addr.sin_port);
    _ip = inet_ntoa(addr.sin_addr);
}

TcpSock::~TcpSock()
{
    close();
}

int TcpSock::init()
{
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if(_sock < 0)
    {
        spdlog::error("error creating socket: {}", strerror(_sock));
        return 1;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if(_ip.empty()) 
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    addr.sin_port = htons(_port);
    int ret = bind(_sock, (sockaddr*)&addr, sizeof(addr));
    _port = ntohs(addr.sin_port);
    if(ret < 0)
    {
        spdlog::error("error bind port {}: {}", _port, strerror(ret));
        ::close(_sock);
        return 1;
    }
    
    return 0;
}

void TcpSock::close()
{
    if(_sock > 0) ::close(_sock);
}

int TcpSock::listen(int conn)
{
    int ret = ::listen(_sock, SOMAXCONN);
    if(ret) return ret;
    spdlog::info("server listening on [{}:{}].", _ip.empty() ? "" : _ip, _port);
    return ret;
}

int TcpSock::connect(const string& ip, int port)
{
    sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    remote.sin_addr.s_addr = inet_addr(ip.c_str());

    int ret = ::connect(_sock, (sockaddr*)&remote, sizeof(remote));
    if(ret)
        spdlog::error("error connecting to {}:{}", ip, port);
    return ret;
}

shared_ptr<TcpSock> TcpSock::accept()
{
    socklen_t len = sizeof(sockaddr_in);
    sockaddr_in addr;
    SOCK sock = ::accept(_sock, (sockaddr*)&addr, &len);
    if(sock > 0)
        return make_shared<TcpSock>(sock, addr);
    return nullptr;
}

int TcpSock::send(const string& data)
{
    return ::send(_sock, data.c_str(), data.length(), MSG_NOSIGNAL); //MSG_NOSIGNAL
}

string TcpSock::recv(int buff_len)
{
    char buff[buff_len];
    int n = ::recv(_sock, buff, buff_len, 0);
    if(n <= 0)
    {
        spdlog::debug("error recv msg {}: {}", n, strerror(n));
        return string();
    }
    return string(buff, n);
}
