#pragma once
#include "room.h"
#include "player.h"
#include "../utils/gomoku.h"

class Match
{
protected:
	u64 _id;
    shared_ptr<Room> _room;
    Gomoku _gmk;

    u32 player_ct(const string& player);
	int move(const string& player, int x, int y);
public:
    Match(shared_ptr<Room> room);
    virtual ~Match();

    
    inline int end(){ return _gmk.end();}
    inline shared_ptr<Room> room() { return _room; }
	inline u64 id() { return _id; }

	/* main progress */
	virtual shared_ptr<match_result> run();
};

class MatchExecutor
{
public:
	/* start a match in a new thread, and notice Game when match end */
	virtual void new_match(shared_ptr<Match> match) = 0;
};
