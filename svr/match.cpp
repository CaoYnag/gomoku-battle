#include "match.h"

Match::Match(shared_ptr<Room> room) : _room(room)
{
}

Match::~Match()
{}

u32 Match::player_ct(const string& player)
{
    auto ct = CHESS_BLANK;
    
    if(player == _room->_owner->name) ct = _room->_room->oct;
    if(player == _room->_guest->name) ct = CHESS_SUM - _room->_room->oct;
    return ct;
}

int Match::move(const string& player, int x, int y)
{
    auto ct = player_ct(player);
    if(ct != CHESS_BLACK || ct != CHESS_WHITE)
	return 1; // TODO magic num?
    return _gmk.put(x, y, ct);
}
