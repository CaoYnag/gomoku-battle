#define BOOST_TEST_MODULE test_svr
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
#include "../../svr/game.h"
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include "../../utils/utils.h"
using std::cout;
using std::endl;
using std::vector;

class VoidUser : public Player
{
public:
	VoidUser(const string& name) : Player(0, name, "", 0) {}
	int next_move(u64 mid, int& x, int& y) { return S_OK; }
	void enemy_move(int x, int y) {}
	void game_result(shared_ptr<match_result> rslt) {}
	void notice(u32 stat) {}
};

BOOST_AUTO_TEST_CASE(test_game)
{
	auto game = make_shared<Game>();

    // Register
	BOOST_REQUIRE_EQUAL(S_PLAYER_INVALID_META, game->register_player(make_shared<VoidUser>("")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<VoidUser>("player1")));
	BOOST_REQUIRE_EQUAL(S_PLAYER_EXISTS, game->register_player(make_shared<VoidUser>("player1")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<VoidUser>("player2")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<VoidUser>("player3")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<VoidUser>("player4")));
	BOOST_REQUIRE(game->get_player("player1") && game->get_player("player2") && game->get_player("player3"));
	// now active player: player1 player2 player3 player4

	// Room Create
	BOOST_REQUIRE_EQUAL(S_ROOM_INVALID_META, game->create_room("player1", make_shared<room_t>("", "")));
	BOOST_REQUIRE_EQUAL(S_PLAYER_INVALID, game->create_room("player_n", make_shared<room_t>("room1", "psw1")));
	BOOST_REQUIRE_EQUAL(S_OK, game->create_room("player1", make_shared<room_t>("room1", "psw1")));
	BOOST_REQUIRE_EQUAL(S_ROOM_EXISTS, game->create_room("player2", make_shared<room_t>("room1", "psw")));
	BOOST_REQUIRE_EQUAL(S_PLAYER_BUSY, game->create_room("player1", make_shared<room_t>("room2", "psw")));
	BOOST_REQUIRE_EQUAL(S_OK, game->create_room("player3", make_shared<room_t>("room2")));
	// active room: room1:psw1(player1) room2(room2)
	
	// Room Join
	BOOST_REQUIRE_EQUAL(S_ROOM_ILLEGAL_ACCESS, game->join_room("player2", make_shared<room_t>("room1", "wrong_psw")));
	BOOST_REQUIRE_EQUAL(S_ROOM_NOT_EXISTS, game->join_room("player2", make_shared<room_t>("room_n")));
	BOOST_REQUIRE_EQUAL(S_OK, game->join_room("player2", make_shared<room_t>("room1", "psw1")));
	BOOST_REQUIRE_EQUAL(S_ROOM_FULL, game->join_room("player4", make_shared<room_t>("room1", "psw1")));
	// room1(player1, player2) room2(player3)
	
	// Room Oper
	BOOST_REQUIRE_EQUAL(S_ROOM_ILLEGAL_OPER, game->change_ct("room1", "player2", CHESS_BLACK));
	BOOST_REQUIRE_EQUAL(S_OK, game->change_ct("room1", "player1", CHESS_BLACK));
	BOOST_REQUIRE_EQUAL(S_ROOM_ILLEGAL_OPER, game->change_state("room1", "player1", PLAYER_STATE_READY));
	BOOST_REQUIRE_EQUAL(S_OK, game->change_state("room1", "player2", PLAYER_STATE_READY));
	
	// Room Exit
	BOOST_REQUIRE_EQUAL(S_ROOM_ILLEGAL_OPER, game->exit_room("player3", "room1"));
	BOOST_REQUIRE_EQUAL(S_ROOM_NOT_EXISTS, game->exit_room("player2", "roomn"));
	BOOST_REQUIRE_EQUAL(S_OK, game->exit_room("player1", "room1"));
	BOOST_REQUIRE_EQUAL(S_OK, game->exit_room("player2", "room1"));
	BOOST_REQUIRE(!game->get_room("room1"));
	// now room: room2(player3)
	
	// Unregister
	BOOST_REQUIRE_EQUAL(S_PLAYER_INVALID, game->unregister_player(make_shared<VoidUser>("playern")));
	BOOST_REQUIRE_EQUAL(S_PLAYER_BUSY, game->unregister_player(make_shared<VoidUser>("player3")));
	BOOST_REQUIRE_EQUAL(S_OK, game->unregister_player(make_shared<VoidUser>("player1")));
}
