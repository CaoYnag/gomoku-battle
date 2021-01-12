#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <cstring>
#include <boost/thread.hpp>
#include <ctime>
#include "utils.h"
using namespace std;

const int HB_TIMEOUT = 15;//heart beat time out
const int DEFAULT_REG_PORT = 11111;
const int DEFAULT_DATA_PORT = 22222;

int MAX_CONN = 1;
int port = DEFAULT_REG_PORT;
int dport = DEFAULT_DATA_PORT;//port for data transfer
SOCK reg, data;//reg: a tcp socket for reg and hb  data: a udp socket transfer data with clients
map<string, time_t> hb;//hb of each client
map<string, sockaddr_in> addrs;

string redirect_msg = "R:1:1";
string reject_msg = "X:1:1";

boost::mutex guard;


void init()
{
    reg = socket(AF_INET, SOCK_STREAM, 0);
    data = socket(AF_INET, SOCK_DGRAM, 0);
    if(reg < 0 || data < 0)
    {
        cerr << "Error while creating socket." << endl;
        exit(0);
    }

    sockaddr_in addr, daddr;
    memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(reg, (sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        cerr << "Error while binding port " << port << ", check if some port are in use." << endl;
        exit(0);
    }
    listen(reg, 100);

    memset(&daddr, 0, sizeof(daddr));
	daddr.sin_family = AF_INET;
	daddr.sin_port = htons(dport);
	daddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(data, (sockaddr*)&daddr, sizeof(daddr));
    if(ret < 0)
    {
        cerr << "Error while binding port " << dport << ", check if some port are in use." << endl;
        exit(0);
    }

    char buff[2048];
    snprintf(buff, 2048, "R:1:%d", dport);
    redirect_msg = buff;
}

/*publish data to each client*/
void publish()
{
    sockaddr_in dst;
    memset(&dst, 0, sizeof(dst));
	dst.sin_family = AF_INET;
	dst.sin_port = htons(port);
	dst.sin_addr.s_addr = htonl(INADDR_ANY);
    while(true)
    {
        const char* msg = genData();
        for(auto& p : hb){
            auto key = p.first;
            sendto(data, msg, strlen(msg), MSG_NOSIGNAL, (sockaddr*)&addrs[key], sizeof(addrs[key]));
        }

        sleep(5);
    }
}

/*process conn from clients*/
void process(SOCK cli, sockaddr_in* from)
{
    char buff[2048];

    char cliip[1024];
    memset(cliip, 0, strlen(cliip));
    int cliport = -1;
    getipportfromaddr((sockaddr_in*)from, cliip, 1024, &cliport);
    cout << "new thread recving from " << cliip << ":" << cliport << endl;
    
    snprintf(buff, 2048, "%s:%d", cliip, cliport);
    string key = buff;
    cout << "key: " << key << endl;

    memset(buff, 0, 2048);
    int n = 0;
    while((n = recv(cli, buff, 2048, 0)) > 0)
    {
        cout << "recv from " << cliip << ":" << cliport << " - " << buff << endl;
        PMSG msg = parseMSG(buff);
        switch(msg->type)
        {
            case REG:
            {
                sockaddr_in dst;
                memset(&dst, 0, sizeof(dst));
	            dst.sin_family = AF_INET;
	            dst.sin_port = htons(msg->port);
	            dst.sin_addr.s_addr = inet_addr(msg->ip);
                
                if(hb.size() >= MAX_CONN)
                {
                    cerr << "Conn pool full, REJECT." << endl;
                    int s = sendto(data, reject_msg.c_str(), strlen(reject_msg.c_str()), MSG_NOSIGNAL, (sockaddr*)&dst, sizeof(dst));
                    break;
                }
                else
                {
                    int s = sendto(data, redirect_msg.c_str(), strlen(redirect_msg.c_str()), MSG_NOSIGNAL, (sockaddr*)&dst, sizeof(dst));
                    
                    boost::lock_guard<boost::mutex> lock(guard);
                    addrs[key] = dst;
                    hb[key] = time(nullptr);
                    cout << "new clients occur, " << hb.size() << " clients now." << endl;
                }
                
            }break;
            case HB:
            {
                boost::lock_guard<boost::mutex> lock(guard);
                if(hb.find(key) != hb.end())//not reged or already time out
                    hb[key] = time(nullptr);
            }break;
        }
        
        memset(buff, 0, strlen(buff));
    }
    cout << "exit thread recving from " << cliip << ":" << cliport << endl;
    close(cli);
    boost::lock_guard<boost::mutex> lock(guard);
    hb.erase(key);
}

void regandhb()
{
    while(true)
    {
        sockaddr_in from;
        socklen_t len = sizeof(from);
        SOCK cli = accept(reg, (sockaddr*)&from, &len);
        boost::thread(boost::bind(process, cli, &from)).detach();
    }
}

void updateHB()
{
    while(true)
    {
        time_t now = time(nullptr);
        for(auto& p : hb){
            auto hbt = p.second;
            if((now - hbt) >= (HB_TIMEOUT))//hb timeout
                hb.erase(p.first);
        }
        cout << "refresh heart beat, " << hb.size() << " clients remain." << endl;
        sleep(5);
    }
}

/*recv data from clients*/
void recvData()
{
    char buff[1024];
    sockaddr_in dst;
    socklen_t len = sizeof(dst);
    while(true)
    {
		memset(buff, 0, sizeof(buff));
	    int n = recvfrom(data, buff, 1024, MSG_NOSIGNAL, (sockaddr*)&dst, &len);
        PMSG msg = parseMSG(buff);
        if(msg == nullptr)
            cerr << "invalid msg: " << buff << endl;
        else
            cout << msg->info() << endl;
        
    }
}

int main(int argc, char* argv[])
{
    if(argc >= 2)
        port = atoi(argv[1]);
    if(argc >= 3)
        dport = atoi(argv[2]);
    if(argc >= 4)
        MAX_CONN = atoi(argv[3]);

    init();
    boost::thread(regandhb).detach();
    boost::thread(updateHB).detach();
    boost::thread(recvData).detach();
    publish();
    
    return 0;
}