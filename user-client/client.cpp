#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <boost/thread.hpp>
#include "../utils/net_utils.h"
using namespace std;

const int       DEFAULT_PORT    = 11011;
const string    DEFAULT_HOST    = "localhost";

const string    DEFAULT_NC      = "wlp3s0";


int lbport = DEFAULT_PORT;
string lbip = DEFAULT_HOST;

int dstport = -1;
string dstip = DEFAULT_HOST;

string nc = DEFAULT_NC;

sockaddr_in local, remote;
SOCK cli;
string localip;
int localport;

void init()
{
    cli = socket(AF_INET, SOCK_STREAM, 0);
    if(cli < 0)
    {
        cerr << "[ERROR]cannot create socket." << endl;
        exit(0);
    }

    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(lbport);
    remote.sin_addr.s_addr = inet_addr(lbip.c_str());
}


void regself()
{
    char buff[2048];
    sockaddr_in dst;
    socklen_t len = sizeof(dst);
    int ret = 0;

    if((ret = connect(cli, (sockaddr*)&remote, sizeof(remote))) < 0)
    {
        cerr << "[ERROR]Cannot connect to server, please check configuration and again. error msg:" << strerror(ret) << endl;
        return;
    }

    while(cin >> buff)
    {
        if(buff[0] == 'q')
        {
            cout << "exit" << endl;
            return;
        }
        if((ret = send(cli, buff, strlen(buff), MSG_NOSIGNAL)) < 0)
        {
            cerr << "cannot send reg info. errmsg: " << strerror(ret) << endl;
            return;
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
    regself();

    close(cli);
    return 0;
}