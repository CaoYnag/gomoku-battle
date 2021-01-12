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
char* msg = "Hello,World!";

int main(int argc, char* argv[])
{
    if(argc >= 2)
        group_host = argv[1];
    if(argc >= 3)
        group_port = atoi(argv[2]);
    if(argc >= 4)
        host = argv[3];
    if(argc >= 5)
        msg = argv[4];
    
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
    long ttl = 100;
    ret = setsockopt(ss, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(long));
    if(ret < 0) cerr << "Cannot set mc ttl." << endl;
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(group_port);
    local.sin_addr.s_addr = inet_addr(host.c_str());
    ret = bind(ss, (sockaddr*)&local, sizeof(local));
    if(ret < 0)
    {
        cerr << "Cannot bind address.";
        close(ss);
        return 0;
    }
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(group_port);
    addr.sin_addr.s_addr = inet_addr(group_host.c_str());

    const int BUFF_SIZE = 1024;
    char buff[BUFF_SIZE] = "Hello,World!\n";
    int cnt = 0;
    while(true)
    {
        sprintf(buff, "the %d msg: %s\n", ++cnt, msg);
        ret = sendto(ss, buff, strlen(buff), 0, (sockaddr*)&addr, sizeof(addr));
        if(ret < 0) cerr << "cannot send msg." << endl;
        else cout << "sended: " << buff << endl;
        sleep(1);
    }

    return 0;
}