#define BOOST_TEST_MODULE test_msg
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <string>
#include <variant>
#include "../../serialize/serialize.h"
#include "../../utils/msg.h"
using namespace std;
using namespace io::serialize;

void info(msg_result msg)
{
	// cout << "result: " << (int)msg.status << "-" << msg.result << "-" << msg.result.length() << endl;
}
void info(msg_request msg)
{
	cout << "request: " << msg.target << "-" << msg.oper << endl;
}
void info(msg_reg msg)
{
	cout << "reg: " << msg.name << ": " << msg.name.length() << endl;
}
void info(msg_roomlist msg)
{
	cout << "roomlist: " << msg.rooms.size() << "\n";
	for(auto& room : msg.rooms)
	{
		cout << room .id << " " << room .name << " " << room .psw << " " << room.state << endl;
	}
}
void info(msg_room msg)
{
	cout << "oper: " << (int)msg.type << " " << msg.room.name << " " << msg.room.psw << endl;
}
void info(msg_chess msg)
{
	cout << "chess: " << (int)msg.type << endl;
}
void info(msg_state msg)
{
	cout << "state: " << (int)msg.state << endl;
}
void info(msg_game msg)
{
	cout << "game: " << msg.state << "-" << msg.ex << endl;
}
void info(msg_move msg)
{
	cout << "move: " << msg.chess << "-(" << (int)msg.x << ", " << (int)msg.y << ")" << endl;
}


BOOST_AUTO_TEST_CASE(test_msg_result)
{
	{
		msg_result msg(0, "ok");
		auto j = serialize(msg);
		msg_result rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.status, rslt.status);
		BOOST_REQUIRE_EQUAL(msg.result, rslt.result);
	}
}


BOOST_AUTO_TEST_CASE(test_msg_request)
{
	{
		msg_request msg(0, 0);
		auto j = serialize(msg);
		msg_request rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.target, rslt.target);
		BOOST_REQUIRE_EQUAL(msg.oper, rslt.oper);
	}
}


BOOST_AUTO_TEST_CASE(test_msg_reg)
{
	{
		msg_reg msg("aaa");
		auto j = serialize(msg);
		msg_reg rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.name, rslt.name);
	}
}

BOOST_AUTO_TEST_CASE(test_msg_roomlist)
{
	{
		vector<room_t> rooms;
		rooms.push_back(room_t(0, "room", "psw"));
		rooms.push_back(room_t(1, "room2", "psw3"));
		rooms.push_back(room_t(2, "aaaa", "xxxxx"));

		msg_roomlist msg(rooms);
		auto j = serialize(msg);
		cout << j << endl;
		msg_roomlist rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.rooms.size(), rslt.rooms.size());
		for(int i = 0; i < msg.rooms.size(); ++i)
		{
			BOOST_REQUIRE_EQUAL(msg.rooms[i] .id, rslt.rooms[i] .id);
			BOOST_REQUIRE_EQUAL(msg.rooms[i] .name, rslt.rooms[i] .name);
			BOOST_REQUIRE_EQUAL(msg.rooms[i] .psw, rslt.rooms[i] .psw);
			BOOST_REQUIRE_EQUAL(msg.rooms[i] .state, rslt.rooms[i] .state);
		}
	}
}

BOOST_AUTO_TEST_CASE(test_msg_room)
{
	{
	    msg_room msg(0, room_t(0, "name", "psw"));
		auto j = serialize(msg);
		msg_room rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.type, rslt.type);
		BOOST_REQUIRE_EQUAL(msg.room .id, rslt.room .id);
		BOOST_REQUIRE_EQUAL(msg.room .name, rslt.room .name);
		BOOST_REQUIRE_EQUAL(msg.room .psw, rslt.room .psw);
		BOOST_REQUIRE_EQUAL(msg.room .state, rslt.room .state);
	}
}

/*
BOOST_AUTO_TEST_CASE(test_msg_room_info)
{
	{
		msg_room_info msg(0, "name");
		auto j = serialize(msg);
		msg_room_info rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.type, rslt.type);
		BOOST_REQUIRE_EQUAL(msg.name, rslt.name);
	}
}
*/
BOOST_AUTO_TEST_CASE(test_msg_chess)
{
	{
		msg_chess msg(0);
		auto j = serialize(msg);
		msg_chess rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.type, rslt.type);
	}
}

BOOST_AUTO_TEST_CASE(test_msg_state)
{
	{
		msg_state msg(0);
		auto j = serialize(msg);
		msg_state rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.state, rslt.state);
	}
}

BOOST_AUTO_TEST_CASE(test_msg_game)
{
	{
		msg_game msg(0, 0);
		auto j = serialize(msg);
		msg_game rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.state, rslt.state);
		BOOST_REQUIRE_EQUAL(msg.ex, rslt.ex);
	}
}

BOOST_AUTO_TEST_CASE(test_msg_move)
{
	{
		msg_move msg(0, 0, 0);
		auto j = serialize(msg);
		msg_move rslt;
		deserialize(j, rslt);
		info(rslt);
		BOOST_REQUIRE_EQUAL(msg.chess, rslt.chess);
		BOOST_REQUIRE_EQUAL(msg.x, rslt.y);
		BOOST_REQUIRE_EQUAL(msg.x, rslt.y);
	}
}
