#include "room.h"
#include "def.h"
using namespace std;


Room::Room(shared_ptr<room_t> room, shared_ptr<player_t> player)
    : _room(room), _owner(player), _guest(nullptr)
{
    _owner->state = PLAYER_STATE_OWNER;
	_room->owner = player->name;
	_room->guest = "";
}

Room::~Room()
{
    // nothing to do.
}

string Room::join(shared_ptr<player_t> user, const string& psw)
{
	lock_guard lock(_guard);
    if(user == _owner || user == _guest)
	return "already in room";
    if(_guest) return "room full";
    if(psw != this->psw) return "wrong psw";
    _guest = user;
    _guest->state = PLAYER_STATE_PREPARE;
    return "";
}

u32 Room::leave(shared_ptr<player_t> user)
{
    user->state = PLAYER_STATE_PREPARE;
    if(user == _guest)
    {
	_guest = nullptr;
	return 1;
    }
    if(user == _owner)
    {
	if(_guest)
	{
	    _owner = _guest;
	    _guest = nullptr;
	    _owner->state = PLAYER_STATE_OWNER;
	    // so now, the new owner use another chess type.
	    return 1;
	}
	else return 0;
    }
    // just return 1 to avoid room destroy
    return 1;
}

u32 Room::change_ct(shared_ptr<player_t> user, int ct)
{
	// complete this later
	return 0;
}

u32 Room::change_state(shared_ptr<player_t> user, int state)
{
	// complete this later
	return 0;
}
