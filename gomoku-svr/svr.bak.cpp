#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <boost/thread.hpp>
#include <ctime>
#include <map>
#include "../utils/net_utils.h"
#include "../utils/config.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
// user defined types logging by implementing operator<<
#include "spdlog/fmt/ostr.h" // must be included
using namespace std;

class Server
{
protected:
    /* Svr Configuration */
    int MAX_CONN;           // max connection svr accept.
    int MAX_CONN_PER_IP;    // max connection from same ip(avoid attack.)
    int _port;              // svr port
    int _oper_timeout;      // timeout for each operation.
protected:
    SOCK _svr, _ctrl;
    int _conn;
    map<string, int> _cons;
    boost::mutex _guard;

    int init_params(const char* path)
    {
        auto conf = spes::conf::get(path);
        if(!conf)
        {
            spdlog::error("error read conf file [{}]", path);
            return 1;
        }

        _port = conf->GetInt("port");
        MAX_CONN = conf->GetInt("max-connections");
        MAX_CONN_PER_IP = conf->GetInt("max-connection-per-ip");
        _oper_timeout = conf->GetInt("operation-timeout");

        spdlog::info("svr configurations:\n\tport: {}\n\tconn limit: {} {}\n\toper timeout: {}s.\n",
        _port, MAX_CONN, MAX_CONN_PER_IP, _oper_timeout);

        return 0;
    }

    int init_svr()
    {
        _svr = socket(AF_INET, SOCK_STREAM, 0);
        if(_svr < 0)
        {
            spdlog::error("error creating socket: {}", strerror(_svr));
            return 1;
        }

        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        int ret = bind(_svr, (sockaddr*)&addr, sizeof(addr));
        if(ret < 0)
        {
            spdlog::error("error bind port {}: {}", _port, strerror(ret));
            close(_svr);
            return 1;
        }
        listen(_svr, SOMAXCONN);
        spdlog::info("server listening in [{}].", _port);
        return 0;
    }
public:
    Server()
    {}
    virtual ~Server()
    {
        close(_svr);
    }

    int init(const char* path)
    {
        boost::lock_guard<boost::mutex> lock(_guard);
        if(init_params(path)) return 1;
        if(init_svr()) return 1;
        return 0;
    }


    void process(SOCK cli, sockaddr_in* from)
    {
        char cliip[1024];
        memset(cliip, 0, strlen(cliip));
        int cliport = -1;
        getipportfromaddr((sockaddr_in*)from, cliip, 1024, &cliport);
        spdlog::info("recv conn from {}:{}", cliip, cliport);

        char buff[2048];
        memset(buff, 0, 2048);
        int n = 0;
        while((n = recv(cli, buff, 2048, 0)) > 0)
        {
            spdlog::info("rcv: {}", buff);
            memset(buff, 0, strlen(buff));
        }

        close(cli);
        spdlog::info("close conn from {}:{}", cliip, cliport);
    }

    void service()
    {
        spdlog::info("server started.");
        while(true)
        {
            sockaddr_in from;
            socklen_t len = sizeof(from);
            SOCK cli = accept(_svr, (sockaddr*)&from, &len);
            boost::thread(boost::bind(&Server::process, this, cli, &from)).detach();
        }
    }
};


void init_log()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S %t][%l] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("server.log", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger)); 
}

int main(int argc, char* argv[])
{
    const char* conf_path = "svr.conf.json";
    if(argc > 1)
        conf_path = argv[1];

    init_log();
    Server svr;
    if(svr.init(conf_path))
    {
        spdlog::error("error start server, exiting...");
        return 1;
    }

    svr.service();
    
    return 0;
}