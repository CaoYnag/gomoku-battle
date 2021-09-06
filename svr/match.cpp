#include "match.h"
#include "game.h"
#include <spes/utils.h>

Match::Match(shared_ptr<Room> room) : _id(room->_room->id), _room(room)
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

shared_ptr<match_result> Match::run()
{
	auto rslt = make_shared<match_result>();
	rslt->begin = cur_ms();
	
	auto owner = _room->_owner;
	auto guest = _room->_guest;
	decltype(owner) p[2];
	u32 ct[2] = {CHESS_BLACK, CHESS_WHITE};
	if(_room->_room->oct == CHESS_BLACK)
	{
		p[0] = owner;
		p[1] = guest;
	}
	else
	{
		p[0] = guest;
		p[1] = owner;
	}

	int cur = 0;
	int x, y;
	int err = 0;
	int step = 0;
	while(!_gmk.end())
	{
		x = -1;
		y = -1;
		
		p[cur]->next_move(_id, x, y);
		if(_gmk.put(x, y, ct[cur]))
		{
			p[cur]->notice(S_MATCH_ILLEGAL_MOVE);
			if(++err > MAX_ILLEGAL_MOVE)
			{
				// reach max illegal move.
				// assume player cannot continue this match.
				// interrupt match.
				break;
			}
			continue;
		}
		++step;
		err = 0; // recover err cnt.
		p[cur]->notice(S_OK);

		cur = 1 - cur;
		p[cur]->enemy_move(x, y);
	}

	rslt->end = cur_ms();
	rslt->steps = step;
	if(err)
		rslt->result = MATCH_RESULT_ERROR;
	else
	{
		// cur lose this match
		if(p[cur] == owner)
			rslt->result = MATCH_RESULT_GUEST_WIN;
		else
			rslt->result = MATCH_RESULT_OWNER_WIN;
	}
	p[0]->game_result(rslt);
	p[1]->game_result(rslt);
	return rslt;
}
