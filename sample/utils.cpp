#include "utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <cstdio>
#include <cstring>
#include <errno.h>
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

string MSG::info()
{
    string ret;
    char buff[2048] = "";
    switch (type)
    {
        case REG:
        {
            snprintf(buff, 2048, "REG:%s:%d", ip, port);
        }break;
        case HB:
        {
            snprintf(buff, 2048, "HB");
        }break;
        case DATA:
        {
            snprintf(buff, 2048, "DATA: %s", data);
        }break;
        case REDIRECT:
        {
            snprintf(buff, 2048, "REDIRECT:%s:%d", ip, port);
        }break;
    
        default:snprintf(buff, 2048, "Invalid message.");
            break;
    }
    ret = buff;
    return ret;
}

PMSG parseMSG(const char* buff)
{
    PMSG msg = new MSG();

    string data = buff;
    switch (buff[0])
    {
        case 'S': //reg
        {
            int s = data.find(":");
            int e = data.find(":", s + 1);
            strcpy(msg->ip, data.substr(s + 1, e - s - 1).c_str());
            msg->port = atoi(data.substr(e + 1, data.size() - e - 1).c_str());
            msg->type = REG;
        }break;
        case 'H': //heart beat
        {
            msg->type = HB;
        }break;
        case 'X': //heart beat
        {
            msg->type = REJECT;
        }break;
        case 'R': //heart beat
        {
            int s = data.find(":");
            int e = data.find(":", s + 1);
            strcpy(msg->ip, data.substr(s + 1, e - s - 1).c_str());
            msg->port = atoi(data.substr(e + 1, data.size() - e - 1).c_str());
            msg->type = REDIRECT;
        }break;
        case 'D': //heart beat
        {
            int s = data.find(":");
            strcpy(msg->data, data.substr(s + 1, data.size() - s - 1).c_str());
            msg->type = DATA;
        }break;
    
        default:
        return nullptr;
            break;
    }
    return msg;
}

const char* genData()
{
    const char* strs[] = 
    {
        "D:Hello,World!",
        "D:Winter is comming",
        "D:We are under attack!!",
        "D:RUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!",
        "D:HAHAHAHAHAHAHAHAHAHAHAHAHA"
    };

    return strs[rand() % 5];
}