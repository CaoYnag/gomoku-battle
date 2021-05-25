#pragma once
#include "room.h"
#include "../utils/gomoku.h"

class Match
{
protected:
    shared_ptr<Room> _room;
    Gomoku _gmk;

    u32 player_ct(const string& player);
public:
    Match(shared_ptr<Room> room);
    virtual ~Match();

    int move(const string& player, int x, int y);
    inline int end(){ return _gmk.end();}
    inline shared_ptr<Room> room() { return _room; }
};
