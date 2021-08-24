/*
 * fundaments of game svr
 * includes some base structure of game.
 */
#pragma once
#include "../utils/def.h"
#include "player.h"
#include <memory>
#include <mutex>
using namespace std;

class Room
{
public:
	shared_ptr<room_t> _room;
    shared_ptr<Player> _owner;
    shared_ptr<Player> _guest;
    mutex _guard;

public:
	/* helper functions */
	
public:
    Room(shared_ptr<room_t> room, shared_ptr<Player> owner);
    ~Room();

	/*
	 * S_ROOM_FULL for room full
	 * S_ROOM_ILLEGAL_PSW for wrong psw
	 * S_ROOM_ALREADY_INSIDE for player already owner or guest.
	 */
    STATUS_CODE join(shared_ptr<Player> user, const string& psw);

	/*
	 * S_ROOM_ILLEGAL_OPER for not in room.
	 * S_ROOM_EMPTY for room empty, game should destroy this room later.
	 */
    STATUS_CODE leave(shared_ptr<Player> user);

	/*
	 * S_ILLEGAL_OPER for not owner change ct, or invalid ct.
	 */
	STATUS_CODE change_ct(shared_ptr<Player> user, u32 ct);

	/*
	 * S_ILLEGAL_OPER for not guest change state, or invalid state.
	 */
	STATUS_CODE change_state(shared_ptr<Player> user, u32 state);
};
