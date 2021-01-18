#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <spdlog/spdlog.h>
#include "server.h"
#include "../utils/config.h"


Server::Server() : _running(0)
{}
Server::~Server()
{
    _svr->close();
}

int Server::init_params(const char* conf_path)
{
    auto conf = spes::conf::get(conf_path);
    if(!conf)
    {
        spdlog::error("error read conf file [{}]", conf_path);
        return 1;
    }

    PORT = conf->GetInt("port");
    MAX_CONN = conf->GetInt("max-connections");
    MAX_CONN_PER_IP = conf->GetInt("max-connection-per-ip");
    OPER_TIMEOUT = conf->GetInt("operation-timeout");

    spdlog::info("svr configurations:\n\tport: {}\n\tconn limit: {} {}\n\toper timeout: {}s.\n",
        PORT, MAX_CONN, MAX_CONN_PER_IP, OPER_TIMEOUT);

    return 0;
}
int Server::init_svr()
{
    _svr = make_shared<MsgSock>(PORT);
    _svr->listen();
    spdlog::info("server listening in [{}].", PORT);
    return 0;
}

int Server::init(const char* conf_path)
{
    boost::lock_guard<boost::mutex> lock(_guard);
    if(init_params(conf_path)) return 1;
    if(init_svr()) return 1;
    _running = 1;
    return 0;
}

void Server::service()
{
    while(_running)
    {
    }
}

void Server::shutdown()
{

}