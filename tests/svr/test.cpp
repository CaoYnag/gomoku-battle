#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
#include "../../svr/game.h"
#include <functional>
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

void test_except(const function<void()>& foo)
{
    try
    {
	foo();
    } catch(exception& ex)
    {
	spdlog::error(ex.what());
    }
}

void test_game()
{
    auto game = Game::get();
    auto p1 = make_shared<player_t>("p1");
    auto p2 = make_shared<player_t>("p2");
    auto p3 = make_shared<player_t>("p3");
    
    p1 = game->register_player(p1);
    p2 = game->register_player(p2);
    p3 = game->register_player(p3);

    if(!p1 || !p2 || !p3)
    {
	spdlog::error("failed register player.");
	return;
    }
    auto r = make_shared<room_t>("r1", "psw");
    r = game->create_room(p1->name, r);

    // try nullptr
    test_except([&](){game->create_room("p2", nullptr);});
    // try invalid name
    test_except([&](){game->create_room("p2", make_shared<room_t>("", ""));});
    // try create room by unexists player
    test_except([&](){game->create_room("??", make_shared<room_t>("rr", "pp"));});
    // try create exists room
    test_except([&](){game->create_room("p2", make_shared<room_t>("", ""));});

    // try join unexists room
    test_except([&](){game->join_room("p2", make_shared<room_t>("aa", "pp"));});
    // try wrong psw
    test_except([&](){game->join_room("p2", make_shared<room_t>("r1", "psww"));});

    // this should success.
    game->join_room("p2", make_shared<room_t>("r1", "psw"));

    // try join full room.
    test_except([&](){game->join_room("p3", make_shared<room_t>("r1", "psw"));});

    // owner exit room
    game->exit_room("p1", "r1");

    // p2 become owner, exit too
    game->exit_room("p2", "r1");

    // so now room destroyed.
    test_except([&](){game->join_room("p2", make_shared<room_t>("r1", "psw"));});

    // create new one
    game->create_room("p2", make_shared<room_t>("r1", "psw"));

    // not-in-room player exit room
    test_except([&](){game->exit_room("p3", "r1");});

    // player join room again.
    game->join_room("p3", make_shared<room_t>("r1", "psw"));

    // guest change ct
    test_except([&](){game->change_ct("r1", "p3", CHESS_WHITE);});

    // wrong ct
    test_except([&](){game->change_ct("r1", "p2", CHESS_BLANK);});

    // owner change ct.
    game->change_ct("r1", "p2", CHESS_WHITE);

    // owner change state
    test_except([&](){game->change_state("r1", "p2", PLAYER_STATE_READY);});

    // guest not ready for match
    test_except([&](){game->start_match("r1", "p2");});
    
    // guest change state
    game->change_state("r1", "p3", PLAYER_STATE_READY);

    // guest start game.
    test_except([&](){game->start_match("r1", "p3");});

    // start match
    auto m = game->start_match("r1", "p2");
    m->move("p2", 0, 0);
    m->move("p2", 0, 1);
    m->move("p2", 0, 2);
    m->move("p2", 0, 3);
    m->move("p2", 0, 4);

    game->match_ovr(m);
    
}


int main(int argc, char* argv[])
{
    init_log();
    test_game();
    return 0;
}
