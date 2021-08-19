#include <iostream>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "../../utils/msg_sock.h"
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
	MsgSock sock;
	if(sock.init()) 
	{
		spdlog::error("error init socket.");
		return 1;
	}
	if(sock.connect("127.0.0.1", 7777))
	{
		spdlog::error("error init socket.");
		return 1;
	}
	sock.rslt(0, "msg");
    sock.req_rooms();
    sock.req(0, 0);
    sock.reg("name");
    vector<room_t> rooms;
    rooms.emplace_back(0, "name1", "");
    rooms.emplace_back(0, "name2", "psw2");
    rooms.emplace_back(0, "name3", "psw3");
    sock.roomlist(rooms);
    sock.room_oper(0, rooms[1]); // create/join/exit
    sock.create_room("name4", "psw4");
    sock.join_room("name4", "psw4");
    sock.exit_room("name1");
    sock.user_join_room("user1");
    sock.user_exit_room("user2");
    sock.roominfo(0, "user");
    sock.choose_chess(0);
    sock.set_chess(0);
    sock.snd_chess(0);
    sock.user_state(0);
    sock.game_win();
    sock.game_lose();
    sock.game_draw();
    sock.snd_game(0, 0);
    sock.move(1, 1);
    sock.move(0, 7, 7);
    sock.close();

    return 0;
}
