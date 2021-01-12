#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <boost/thread.hpp>
#include "utils.h"
using namespace std;

const int       DEFAULT_PORT    = 20000;
const string    DEFAULT_HOST    = "localhost";

const string    DEFAULT_NC      = "eth0";


int lbport = DEFAULT_PORT;
string lbip = DEFAULT_HOST;

int dstport = -1;
string dstip = DEFAULT_HOST;

string nc = DEFAULT_NC;

sockaddr_in local, remote;
SOCK reg, data;
string localip;
int localport;
enum ClientState{S_REG, S_TRANS};
int state = S_REG;
void init()
{
    reg = socket(AF_INET, SOCK_STREAM, 0);
    data = socket(AF_INET, SOCK_DGRAM, 0);
    timeval to = {3, 0};
    setsockopt(data, SOL_SOCKET, SO_SNDTIMEO, (const char*)&to, sizeof(to));
    setsockopt(data, SOL_SOCKET, SO_RCVTIMEO, (const char*)&to, sizeof(to));
    if(reg < 0 || data < 0)
    {
        cerr << "Cannot create socket." << endl;
        exit(0);
    }

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(4000);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = -1;
    localport = 9999;
    while(ret < 0)
    {
        local.sin_port=htons(++localport);
        ret = bind(data, (sockaddr*)&local, sizeof(local));
    }

    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(lbport);
    remote.sin_addr.s_addr = inet_addr(lbip.c_str());

    localip = getlocalip(nc.c_str());
}


void progress()
{
    char buff[1024];
    sockaddr_in dst;
    socklen_t len = sizeof(dst);

    while(true)
    {
        if(state == S_REG)
        {
            cout << "Reging, stop data trans." << endl;
            sleep(1);
            continue;
        }
        memset(buff, 0, sizeof(buff));
	    int ret = recvfrom(data, buff, 1024, MSG_NOSIGNAL, (sockaddr*)&dst, &len);
        if(ret < 0)
        {
            cerr << "Cannot recv any data." << endl;
            continue;
        }
        PMSG msg = parseMSG(buff);
        if(msg != nullptr && msg->type == DATA)
        {
            cout << msg->info() << endl;
        }
        else 
            cerr << "invalid msg: " << buff << endl;

        ret = sendto(data, buff, strlen(buff), MSG_NOSIGNAL, (sockaddr*)&dst, sizeof(dst));
        if(ret < 0)
        {
            cerr << "Cannot send any data." << endl;
            continue;
        }
    }
}

void regself()
{
    char buff[2048] = "s";

    char recv[2048];
    
    sockaddr_in dst;
    socklen_t len = sizeof(dst);
    int ret = 0;
    while(true)
    {
        snprintf(buff, 2048, "S:%s:%d", localip.c_str(), localport);
        cout << "reging: " << buff << endl;
        while(state == S_REG)
        {
            sleep(1);
            close(reg);
            reg = socket(AF_INET, SOCK_STREAM, 0);
            if(reg < 0)
            {
                cerr << "Cannot get tcp socket." << endl;
                exit(0);
            }

            if((ret = connect(reg, (sockaddr*)&remote, sizeof(remote))) < 0)
            {
                cerr << "Cannot connect to server, please check configuration and again. error msg:" << strerror(ret) << endl;
                continue;
            }
            if((ret = send(reg, buff, strlen(buff), MSG_NOSIGNAL)) < 0)
            {
                cerr << "cannot send reg info. errmsg: " << strerror(ret) << endl;
                continue;
            }

            memset(recv, 0, sizeof(recv));
            if((ret = recvfrom(data, recv, 2048, MSG_NOSIGNAL, (sockaddr*)&dst, &len)) < 0)
            {
                cerr << "cannot recv msg after reg, errmsg: " << strerror(ret) << endl;
                continue;
            }
            cout << recv << endl;
            PMSG msg = parseMSG(recv);
            if(msg != nullptr && msg->type == REDIRECT)
            {
                cout << "Redirect to " << msg->ip << ":" << msg->port << endl;
                state = S_TRANS;
                break;
            }
            else if(msg != nullptr && msg->type == REJECT)
                cerr << "Reject from server, try reg again: ";
            else 
                cerr << "invalid msg: " << recv << endl;
        }

        snprintf(buff, 2048, "H:%s:%d", localip.c_str(), localport);
        while(state == S_TRANS)
        {
            if((ret = send(reg, buff, strlen(buff), MSG_NOSIGNAL)) < 0)
            {
                cerr << "Sth error, try reconnect." << endl;
                state = S_REG;
                continue;
            }
            
            sleep(5);//send hb every 5s
        }
    }
}


int main(int argc, char* argv[])
{
    if(argc >= 2)
        lbip = argv[1];
    if(argc >= 3)
        lbport = atoi(argv[2]);
    cout << "LB loc: " << lbip << ":" << lbport << endl;
    if(argc >= 4)
        nc = argv[3];
    
    init();
    boost::thread(regself).detach();
    boost::thread(progress).detach();
    while(true)sleep(1000);
    return 0;
}