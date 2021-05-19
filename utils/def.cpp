#include "def.h"


player_t::player_t() {}
player_t::player_t(u32 d, const string& n, u32 s, const string& i, u32 p)
    : id(d), name(n), state(s), ip(i), port(p)
{}

room_t::room_t() : id(-1), oct(CHESS_WHITE), state(ROOM_STATE_OPEN)
{}
room_t::room_t(const string& n, const string& p)
    : id(-1), name(n), psw(p), oct(CHESS_WHITE), state(ROOM_STATE_OPEN)
{}
room_t::room_t(u32 i, const string& n, const string& p, u32 t, u32 s)
    : id(i), name(n), psw(p), oct(t), state(s)
{}
room_t::room_t(const room_t& room)
    : id(room.id), name(room.name), psw(room.psw), oct(room.oct), state(room.state)
{}
