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

BOOST_AUTO_TEST_CASE(test_game)
{
	auto game = Game::get();

    // Register
	BOOST_REQUIRE_EQUAL(S_PLAYER_INVALID_NAME, game->register_player(make_shared<player_t>("")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<player_t>("player1")));
	BOOST_REQUIRE_EQUAL(S_PLAYER_EXISTS, game->register_player(make_shared<player_t>("player1")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<player_t>("player2")));
	BOOST_REQUIRE_EQUAL(S_OK, game->register_player(make_shared<player_t>("player3")));
	BOOST_REQUIRE(game->get_player("player1") && game->get_player("player2") && game->get_player("player3"));

	// Room
	
	// Unregister
	BOOST_REQUIRE_EQUAL(S_PLAYER_INVALID, game->unregister_player(make_shared<player_t>("player4")));
}
