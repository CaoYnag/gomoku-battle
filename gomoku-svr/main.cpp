#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
// user defined types logging by implementing operator<<
#include "spdlog/fmt/ostr.h" // must be included
#include "server.h"
using namespace std;


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