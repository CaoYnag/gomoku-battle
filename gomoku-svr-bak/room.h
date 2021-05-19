/*
 * fundaments of game svr
 * includes some base structure of game.
 */
#pragma once
#include "../utils/def.h"
#include <memory>
using namespace std;

class Room : public room_t
{
public:
    shared_ptr<player_t> _owner;
    shared_ptr<player_t> _guest;
public:
    Room(u32 i, const string& n, const string& p, u32 s, shared_ptr<player_t> owner);
    ~Room();

    /*
     * empty string for join succ.
     * or msg why join failed.
     */
    string join(shared_ptr<player_t> user, const string& psw);
    /* 
     * return players remain.
     * so if `0` is returned, this room need to be destroy
     */
    u32 leave(shared_ptr<player_t> user);
    
};
