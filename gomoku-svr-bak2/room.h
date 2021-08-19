/*
 * fundaments of game svr
 * includes some base structure of game.
 */
#pragma once
#include "../utils/def.h"
#include <memory>
#include <mutex>
using namespace std;

class Room
{
public:
	shared_ptr<room_t> _room;
    shared_ptr<player_t> _owner;
    shared_ptr<player_t> _guest;
	mutex _guard;
public:
    Room(shared_ptr<room_t> room);
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

	// return status code
	u32 change_ct(shared_ptr<player_t> user, int ct);
	// return status code
	u32 change_state(shared_ptr<player_t> user, int state);
};
