#include "def.h"

string str_status_code(STATUS_CODE code)
{
	switch(code)
	{
	case S_OK: return "OK";
	case S_NOCHANGE: return "no chanage";
	case S_ERROR: return "general error";
	case S_ILLEGAL_OPER_LIMITS: return "reach max error limits";
	case S_INVALID_MSG: return "invalid msg";
	case S_ILLEGAL_MSG: return "illegal msg";
	case S_ILLEGAL_OPER: return "illegal operation";

	case S_PLAYER_INVALID_META: return "invalid player info";
	case S_PLAYER_EXISTS: return "player already exists";
	case S_PLAYER_INVALID: return "invalid player";
	case S_PLAYER_BUSY: return "player busy";

	case S_ROOM_INVALID_META: return "invalid room info";
	case S_ROOM_EXISTS: return "room already exists";
	case S_ROOM_NOT_EXISTS: return "room not exists";
	case S_ROOM_FULL: return "room full";
	case S_ROOM_ILLEGAL_ACCESS: return "illegal access(wrong psw)";
	case S_ROOM_ALREADY_INSIDE: return "already inside room";
	case S_ROOM_ILLEGAL_OPER: return "illegal operation";
	case S_ROOM_EMPTY: return "room empty";

	case S_GAME_NO_GUEST: return "need guest";
	case S_GAME_ILLEGAL_OPER: return "illegal operation";
	case S_GAME_NOT_PREPARE: return "guest not ready";

	case S_MATCH_ILLEGAL_MOVE: return "illegal movement";
	case S_MATCH_OPER_TIMEOUT: return "timeout";
	default:
		return "unkown error.";
	}
}

player_records::player_records() : total(0), win(0), lose(0), avg_step(.0f)
{}
void player_records::game_win(int step)
{
	update_total_and_step(step);
	win += 1;
}
void player_records::game_lose(int step)
{
	update_total_and_step(step);
	lose += 1;
}
void player_records::game_draw(int step)
{
	update_total_and_step(step);
}
void player_records::update_total_and_step(int step)
{
	f32 tt = total * avg_step;
	tt += step;
	total += 1;
	avg_step = tt / total;
}

player_t::player_t()
    : id(INVALID_ID), name(""), state(PLAYER_STATE_IDLE), ip(""), port(0) {}
player_t::player_t(const string& n)
    : id(0), name(n), state(PLAYER_STATE_IDLE), ip(""), port(0) {}
player_t::player_t(u64 d, const string& n, u32 s, const string& i, u32 p)
    : id(d), name(n), state(s), ip(i), port(p) {}

room_t::room_t() : id(INVALID_ID), oct(CHESS_WHITE), state(ROOM_STATE_OPEN)
{}
room_t::room_t(const string& n)
	: id(INVALID_ID), name(n), oct(CHESS_WHITE), gs(PLAYER_STATE_PREPARE), state(ROOM_STATE_OPEN)
{}
room_t::room_t(const string& n, const string& p)
    : id(INVALID_ID), name(n), psw(p), oct(CHESS_WHITE), gs(PLAYER_STATE_PREPARE), state(ROOM_STATE_OPEN)
{}
room_t::room_t(u64 i, const string& n, const string& p)
    : id(i), name(n), psw(p), oct(CHESS_WHITE), gs(PLAYER_STATE_PREPARE), state(ROOM_STATE_OPEN)
{}
room_t::room_t(const room_t& room)
    : id(room.id), name(room.name), psw(room.psw), oct(room.oct), state(room.state),
	  gs(room.gs), owner(room.owner), guest(room.guest)
{}
