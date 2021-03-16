#pragma once
#include <string>
#include <vector>
#include <rttr/registration>
#include "def.h"
#include "../serialize/serialize.h"
using namespace std;
using namespace rttr;
using namespace io::serialize;

typedef string msg_raw_t;

constexpr const u32 MSG_T_UNKNOWN    = 0x0;
constexpr const u32 MSG_T_RESULT     = 0x1;
constexpr const u32 MSG_T_REQUEST    = 0x2;
constexpr const u32 MSG_T_REGISTER   = 0x3;
constexpr const u32 MSG_T_ROOM_LIST  = 0x4;
constexpr const u32 MSG_T_ROOM_OPER  = 0x5;
constexpr const u32 MSG_T_ROOM_INFO  = 0x6;
constexpr const u32 MSG_T_CHESS      = 0x7;
constexpr const u32 MSG_T_STATE      = 0x8;
constexpr const u32 MSG_T_GAME       = 0x9;
constexpr const u32 MSG_T_MOVE       = 0xa;

struct msg_t
{
    RTTR_ENABLE()
public:
    u32 msg_type;

    msg_t();
    msg_t(u32 type);
};

/* 
* result for some operation like reg, create, join and other.
* status 0 for succ. and may not contain a result field.
* other status value means sth happened
*/
struct msg_result : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 status;
    string result;

    msg_result();
    msg_result(u32 s, const string& r);
};

struct msg_request : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 target;
    u32 oper;

    msg_request();
    msg_request(u32 t, u32 o);
};

struct msg_reg : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    string name;

    msg_reg();
    msg_reg(const string& name);
};

struct msg_roomlist : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    vector<room_t> rooms;

    msg_roomlist();
    msg_roomlist(const vector<room_t>& rooms);
};

struct msg_room_oper : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 type;       // create/join
    room_t room;

    msg_room_oper();
    msg_room_oper(u32 t, const room_t& r);
};

/* player join or exit */
struct msg_room_info : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 type;
    string name;

    msg_room_info();
    msg_room_info(u32 t, const string& n);
};

/* choose chess type */
struct msg_chess : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 type;

    msg_chess();
    msg_chess(u32 t);
};

/* 
* player state change.
* ready or cancel ready
*/
struct msg_state : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 state;

    msg_state();
    msg_state(u32 s);
};

/*
* game states, like start, end
*/
struct msg_game : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 state;
    u32 ex;
    string message;

    msg_game();
    msg_game(u32 s, u32 e);
    msg_game(u32 s, u32 e, const string& m);
};

struct msg_move : public msg_t
{
    RTTR_ENABLE(msg_t)
public:
    u32 chess;
    u32 x, y;

    msg_move();
    msg_move(u32 xx, u32 yy);
    msg_move(u32 c, u32 xx, u32 yy);
};

template<class T>
msg_raw_t pack(const T& msg)
{
    try
    {
        return serialize(msg);
    }
    catch(const std::exception& e)
    {
        return string();
    }
    
}
u32 msg_type(const msg_raw_t& raw);
template<class T>
u32 unpack(const msg_raw_t& raw, T& msg)
{
    if(raw.empty()) return 1;
    try
    {
        deserialize(raw, msg);
    }catch(exception& e)
    {
        return 1;
    }
    return 0;
}
shared_ptr<msg_result> unpack_result(const msg_raw_t& raw);
shared_ptr<msg_request> unpack_request(const msg_raw_t& raw);
shared_ptr<msg_reg> unpack_reg(const msg_raw_t& raw);
shared_ptr<msg_roomlist> unpack_roomlist(const msg_raw_t& raw);
shared_ptr<msg_room_oper> unpack_roomoper(const msg_raw_t& raw);
shared_ptr<msg_room_info> unpack_roominfo(const msg_raw_t& raw);
shared_ptr<msg_chess> unpack_chess(const msg_raw_t& raw);
shared_ptr<msg_state> unpack_state(const msg_raw_t& raw);
shared_ptr<msg_game> unpack_game(const msg_raw_t& raw);
shared_ptr<msg_move> unpack_move(const msg_raw_t& raw);


RTTR_REGISTRATION
{
    registration::class_<player_t>("Player")
	.constructor<>()
	.property("id", &player_t::id)
	.property("name", &player_t::name)
	.property("ip", &player_t::ip)
	.property("port", &player_t::port);
    registration::class_<room_t>("Room")
	.constructor<>()
	.property("id", &room_t::id)
	.property("name", &room_t::name)
	.property("psw", &room_t::psw)
	.property("state", &room_t::state);
    
    registration::class_<msg_t>("Msg")
	.constructor<>()
	.property("msg_type", &msg_t::msg_type);
    registration::class_<msg_result>("Result")
	.constructor<>()
	.property("status", &msg_result::status)
	.property("result", &msg_result::result);
    registration::class_<msg_request>("Request")
	.constructor<>()
	    .property("oper", &msg_request::oper)
	.property("target", &msg_request::target);
    registration::class_<msg_reg>("Register")
	.constructor<>()
	.property("name", &msg_reg::name);
    registration::class_<msg_roomlist>("RoomList")
	.constructor<>()
	.property("rooms", &msg_roomlist::rooms);
    registration::class_<msg_room_oper>("RoomOper")
	.constructor<>()
	.property("type", &msg_room_oper::type)
	.property("room", &msg_room_oper::room);
    registration::class_<msg_room_info>("RoomInfo")
	.constructor<>()
	.property("type", &msg_room_info::type)
	.property("name", &msg_room_info::name);
    registration::class_<msg_chess>("Chess")
	.constructor<>()
	.property("type", &msg_chess::type);
    registration::class_<msg_state>("State")
	.constructor<>()
	.property("state", &msg_state::state);
    registration::class_<msg_game>("Game")
	.constructor<>()
	.property("state", &msg_game::state)
	.property("ex", &msg_game::ex)
	.property("message", &msg_game::message);
    registration::class_<msg_move>("Move")
	.constructor<>()
	.property("chess", &msg_move::chess)
	.property("x", &msg_move::x)
	.property("y", &msg_move::y);
}
