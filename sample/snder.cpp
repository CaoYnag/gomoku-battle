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

string pip;//proxy ip
int pport = -1;//proxy port
string ptype;//proxy type, only socks5 now
string puser;//proxy user
string ppsw;//proxy psw
string ptc;//protocol
string dip;//dst ip
int dport = -1;//dst port
string sip;//src ip
int sport = -1;//src port

SOCK ss = -1;

void help_msg()
{
    cerr << "snd [OPTIONS] dip dport\n"
        << "OPTIONS:\n"
        << "\t-t protocol: tcp udp\n"
        << "\t-s src ip\n"
        << "\t-p src port\n"
        << "\t-a proxy auth user:psw\n"
        << "\t-P proxy(ip:port)\n"
        << "\n";
}
void help_and_exit()
{
    help_msg();
    exit(0);
}
void exit(const string msg)
{
    cerr << msg << endl;
    exit(0);
}
void info_check()
{
    if(dip.length() <= 0 || dport < 0)
    {
        cerr << "dst not set." << endl;
        exit(0);
    }
    if(pip.length() > 0 && pport < 0)
    {
        cerr << "invalid proxy: " << pip << ":" << pport << endl;
        exit(0);
    }
}
void init(int argc, char** argv)
{
    cout << "----------INFO-------------\n" << argc << " args\n";
    for(int i = 0; i < argc; ++i)
        cout << i << ":" << argv[i] << "\n";
    cout << "\n---------------------------\n";
    if(argc <= 1)
        help_and_exit();
    for(int i = 1; i < argc;)
    {
        string arg = argv[i];
        if(arg[0] == '-')
        {
            if(arg.length() != 2)
            {
                cerr << "invalid option " << arg << ", u must specified only option once." << endl;
                exit(0);
            }
            if((i + 1) >= argc)
            {
                cerr << "no value set for option: " << arg << "!" << endl;
                exit(0);
            }

            string v = argv[++i];
            switch(arg[1])
            {
                case 't':{
                    ptc = v;
                    if(ptc != "tcp" && ptc != "udp")
                    {
                        cerr << "only tcp and udp supported for option " << arg << ".\n";
                        exit(0);
                    }
                }break;
                case 's':{
                    sip = v;
                }break;
                case 'p':{
                    sport = atoi(v.c_str());
                }break;
                case 'a':{
                    int idx = v.find(':');
                    puser = v.substr(0, idx);
                    ppsw = v.substr(idx + 1, v.length());
                }break;
                case 'P':{
                    int idx = v.find(':');
                    pip = v.substr(0, idx);
                    pport = atoi(v.substr(idx + 1, v.length()).c_str());
                }break;
                default:{
                    cerr << "unsupport option " << arg << endl;
                    exit(0);
                }break;
            }
        }
        else
        {
            //dst ip or port
            static int ip = 1;
            if(ip)
            {
                dip = arg;
                --ip;
            }
            else if(ip == 0)
            {
                dport = atoi(arg.c_str());
                --ip;
            }
        }
        
        ++i;
    }
    info_check();
}

void setup()
{
    if(ptc == "udp")
        ss = socket(AF_INET, SOCK_DGRAM, 0);
    else
        ss = socket(AF_INET, SOCK_STREAM, 0);
    
    if(ss < 0)
    {
        cerr << "Cannot create socket." << endl;
        exit(0);
    }

    if(pip.length() > 0 && pport > 0)
    {
        sockaddr_in proxy;
        memset(&proxy, 0, sizeof(proxy));
        proxy.sin_addr.s_addr = inet_addr(pip.c_str());
        proxy.sin_family = AF_INET;
        proxy.sin_port = htons(pport);
        connect(ss, (sockaddr*)&proxy, sizeof(proxy));//tcp only?

        uint len = 6 + puser.length() + ppsw.length() + 4;//???
        char* buff = new char[len];
        int idx = 0;
        buff[idx++] = 85;
        buff[idx++] = puser.length();
        snprintf(buff + idx, puser.length(), "%s", puser.c_str());
        idx += puser.length();

    }
}

void doSnd()
{
    setup();
}

int main(int argc, char* argv[])
{
    init(argc, argv);
    doSnd();

    return 0;
}