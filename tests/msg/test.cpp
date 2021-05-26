#define BOOST_TEST_MODULE test_msg
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <string>
#include <variant>
#include "../../utils/msg.h"
using namespace std;

void info(const msg_raw_t& raw)
{
    cout << "raw: " << raw.length() << endl;
}
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
	cout << room.id << " " << room.name << " " << room.psw << " " << room.state << endl;
    }
}
void info(msg_room_oper msg)
{
    cout << "oper: " << (int)msg.type << " " << msg.room.name << " " << msg.room.psw << endl;
}
void info(msg_room_info msg)
{
    cout << "info: " << (int)msg.type << " " << msg.name << endl;
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

BOOST_AUTO_TEST_CASE(test_msg_t)
{
    {
	msg_t msg(0, 0, 0);
	auto raw = pack(msg);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_result)
{
    {
	msg_result msg(0, "ok");
	auto raw = pack(msg);
	auto rslt = unpack_result(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.status, rslt->status);
	BOOST_REQUIRE_EQUAL(msg.result, rslt->result);
    }
    
    {
	msg_result msg(1, "invalid operation.");
	auto raw = pack(msg);
	auto rslt = unpack_result(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.status, rslt->status);
	BOOST_REQUIRE_EQUAL(msg.result, rslt->result);
    }
}


BOOST_AUTO_TEST_CASE(test_msg_request)
{
    {
	msg_request msg(0, 0);
	auto raw = pack(msg);
	auto rslt = unpack_request(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.target, rslt->target);
	BOOST_REQUIRE_EQUAL(msg.oper, rslt->oper);
    }
    
    {
	msg_request msg(1, 1);
	auto raw = pack(msg);
	auto rslt = unpack_request(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.target, rslt->target);
	BOOST_REQUIRE_EQUAL(msg.oper, rslt->oper);
    }
}


BOOST_AUTO_TEST_CASE(test_msg_reg)
{
    {
	msg_reg msg("aaa");
	auto raw = pack(msg);
	auto rslt = unpack_reg(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.name, rslt->name);
    }
    {
	msg_reg msg("dasfsafagasawadsad");
	auto raw = pack(msg);
	auto rslt = unpack_reg(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.name, rslt->name);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_roomlist)
{
    {
	vector<room_t> rooms;
	rooms.push_back(room_t(0, "room", "psw", 0, 0));
	rooms.push_back(room_t(1, "room2", "psw3", 1, 0));
	rooms.push_back(room_t(2, "aaaa", "xxxxx", 3, 0));
	
	msg_roomlist msg(rooms);
	auto raw = pack(msg);
	auto rslt = unpack_roomlist(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.rooms.size(), rslt->rooms.size());
	for(int i = 0; i < msg.rooms.size(); ++i)
	{
	    BOOST_REQUIRE_EQUAL(msg.rooms[i].id, rslt->rooms[i].id);
	    BOOST_REQUIRE_EQUAL(msg.rooms[i].name, rslt->rooms[i].name);
	    BOOST_REQUIRE_EQUAL(msg.rooms[i].psw, rslt->rooms[i].psw);
	    BOOST_REQUIRE_EQUAL(msg.rooms[i].state, rslt->rooms[i].state);
	}
    }
}

BOOST_AUTO_TEST_CASE(test_msg_room_oper)
{
    {
	msg_room_oper msg(0, room_t(0, "name", "psw", 0, 0));
	auto raw = pack(msg);
	auto rslt = unpack_roomoper(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.type, rslt->type);
	BOOST_REQUIRE_EQUAL(msg.room.id, rslt->room.id);
	BOOST_REQUIRE_EQUAL(msg.room.name, rslt->room.name);
	BOOST_REQUIRE_EQUAL(msg.room.psw, rslt->room.psw);
	BOOST_REQUIRE_EQUAL(msg.room.state, rslt->room.state);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_room_info)
{
    {
	msg_room_info msg(0, "name");
	auto raw = pack(msg);
	auto rslt = unpack_roominfo(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.type, rslt->type);
	BOOST_REQUIRE_EQUAL(msg.name, rslt->name);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_chess)
{
    {
	msg_chess msg(0);
	auto raw = pack(msg);
	auto rslt = unpack_chess(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.type, rslt->type);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_state)
{
    {
	msg_state msg(0);
	auto raw = pack(msg);
	auto rslt = unpack_state(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.state, rslt->state);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_game)
{
    {
	msg_game msg(0, 0);
	auto raw = pack(msg);
	auto rslt = unpack_game(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.state, rslt->state);
	BOOST_REQUIRE_EQUAL(msg.ex, rslt->ex);
    }
    {
	msg_game msg(1, 2);
	auto raw = pack(msg);
	auto rslt = unpack_game(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.state, rslt->state);
	BOOST_REQUIRE_EQUAL(msg.ex, rslt->ex);
    }
}

BOOST_AUTO_TEST_CASE(test_msg_move)
{
    {
	msg_move msg(0, 0, 0);
	auto raw = pack(msg);
	auto rslt = unpack_move(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.chess, rslt->chess);
	BOOST_REQUIRE_EQUAL(msg.x, rslt->x);
	BOOST_REQUIRE_EQUAL(msg.y, rslt->y);
    }
    {
	msg_move msg(1, 7, 14);
	auto raw = pack(msg);
	auto rslt = unpack_move(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg.chess, rslt->chess);
	BOOST_REQUIRE_EQUAL(msg.x, rslt->x);
	BOOST_REQUIRE_EQUAL(msg.y, rslt->y);
    }
}

BOOST_AUTO_TEST_CASE(test_pack_shared_ptr)
{
    {
	auto msg = make_shared<msg_move>(0, 0, 0);
	auto raw = pack(msg);
	auto rslt = unpack_move(raw);
	info(raw);
	info(*rslt);
	BOOST_REQUIRE_EQUAL(msg->chess, rslt->chess);
	BOOST_REQUIRE_EQUAL(msg->x, rslt->x);
	BOOST_REQUIRE_EQUAL(msg->y, rslt->y);
    }
}
