#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
// user defined types logging by implementing operator<<
#include "spdlog/fmt/ostr.h" // must be included
#include "game_svr.h"
#include "def.h"
using namespace std;


void init_log()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifdef __debug__
	cout << "DEBUG MODE ENABLED!" << endl;
	console_sink->set_level(spdlog::level::debug);
#elif
	console_sink->set_level(spdlog::level::info);
#endif
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S %t][%l] %v");
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("gsvr.log", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger)); 
}

void term_svr(int sig)
{
	spdlog::info("rcv int sig, interupting svr...");
	GameSvr::get()->shutdown();
}

int main(int argc, char* argv[])
{
    const char* conf_path = "svr.conf.json";
    if(argc > 1)
        conf_path = argv[1];

    init_log();
    if(GlobalConf::init(conf_path))
    {
        spdlog::error("error init configuraion, exiting...");
        return 1;
    }
    auto svr = GameSvr::get();
    svr->init();

	// reg sig handle before run svr.
	{
		struct sigaction act;
		act.sa_flags = 0;
		act.sa_handler = term_svr;
		sigemptyset(&act.sa_mask);
		sigaction(SIGINT, &act, NULL);
	}
	
    svr->service();
	svr->shutdown();
    
    return 0;
}
