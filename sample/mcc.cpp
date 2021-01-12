#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "utils.h"
using namespace std;

string group_host = "239.0.0.255";
int group_port = 11111;
string host = "127.0.0.1";
SOCK ss = -1;

int main(int argc, char* argv[])
{
    if(argc >= 2)
        group_host = argv[1];
    if(argc >= 3)
        group_port = atoi(argv[2]);
    if(argc >= 4)
        host = argv[3];
    
    ss = socket(AF_INET, SOCK_DGRAM, 0);
    if(ss < 0)
    {
        cerr << "Cannot create socket." << endl;
        return 0;
    }
    int reuse = 1;
    int ret = -1;
    ret = setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
    if(ret < 0)
    {
        cerr << "Cannot reuse socket." << endl;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(group_port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    ret = bind(ss, (sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        cerr << "Cannot bind address.";
        close(ss);
        return 0;
    }

    ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(group_host.c_str());
    group.imr_interface.s_addr = inet_addr(host.c_str());
    ret = setsockopt(ss, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group, sizeof(group));
    if(ret < 0)
    {
        cerr << "cannot add mc group." << endl;
        close(ss);
        return 0;
    }

    const int BUFF_SIZE = 1024;
    char buff[BUFF_SIZE];
    sockaddr_in cli;
    while(true)
    {
        memset(buff, 0, BUFF_SIZE);
        socklen_t len = sizeof(cli);
        ret = recvfrom(ss, buff, BUFF_SIZE, 0, (sockaddr*)&cli, &len);
        if(ret < 0)
        {
            cerr << "error recv msg." << endl;
            return 0;
        }
        cout << buff;
    }

    return 0;
}