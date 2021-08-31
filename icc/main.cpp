#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "csh.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // must be included
using namespace std;

Csh sh;

void init_log()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifdef __debug__
    console_sink->set_level(spdlog::level::debug);
#elif
	console_sink->set_level(spdlog::level::info);
#endif
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S %t][%l] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("icc.log", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger)); 
}


void term_svr(int sig)
{
	sh.close();
}

int main(int argc, char* argv[])
{
	init_log();
	// reg int handler
	{
		struct sigaction act;
		act.sa_flags = 0;
		act.sa_handler = term_svr;
		sigemptyset(&act.sa_mask);
		sigaction(SIGINT, &act, NULL);
	}

	sh.run();
    return 0;
}
