/**
Basic defination of player in game svr.
*/
#pragma once
#include "../utils/def.h"
#include <memory>
using namespace std;

class Player : public player_t
{
protected:
	shared_ptr<player_records> _stats;
public:
	Player(u64 id, const string& name, const string& ip, u32 port);
	virtual ~Player();

	inline shared_ptr<player_records> stats() { return _stats; }
public:
	/** match related intfs **/
	/* match call this method to got player next move */
	virtual int next_move(u64 mid, int& x, int& y) = 0;
	/* a message intf, match call this to notice player enemy movement */
	virtual void enemy_move(int x, int y) = 0;
	/* a message intf, match call this to notice player operation result, eg: illegal move */
	virtual void notice(u32 stat) = 0;
	/* a message intf used to notice player result of a match */
	virtual void game_result(shared_ptr<match_result> rslt) = 0;
};
