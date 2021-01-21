#include "def.h"


player_t::player_t() {}
player_t::player_t(u32 d, const string& n, const string& i, u32 p)
    : id(d), name(n), ip(i), port(p)
{}
player_t::player_t(u32 d, const string& n)
    : id(d), name(n)
{}

room_t::room_t() {}
room_t::room_t(u32 i, const string& n, const string& p, u32 s)
    : id(i), name(n), psw(p), state(s)
{}