#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "../../utils/net_utils.h"
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

int main()
{
	TcpSock sock(7777);
	if(sock.init()) 
	{
		spdlog::error("error init socket.");
		return 1;
	}
	if(sock.listen()) 
	{
		spdlog::error("error listen.");
		return 1;
	}
	auto cli = sock.accept();
	spdlog::info("recv conn from {}:{}", cli->ip(), cli->port());
	spdlog::info("recv msg: {}", cli->recv());
	cli->send("42");
	cli->close();
	sock.close();

	return 0;
}