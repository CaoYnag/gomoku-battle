#include "room.h"
#include "game.h"
using namespace std;

/*====================Room====================*/
Room::Room(shared_ptr<room_t> room, shared_ptr<player_t> owner)
    : _room(room), _owner(owner), _guest(nullptr)
{
    _owner->state = PLAYER_STATE_OWNER;
	_room->owner = owner->name;
	_room->guest = "";
}

Room::~Room()
{
    // nothing to do.
}

STATUS_CODE Room::join(shared_ptr<player_t> user, const string& psw)
{
	lock_guard<mutex> lock(_guard);
    if(user == _owner || user == _guest)
		return S_ROOM_ALREADY_INSIDE;
    if(_guest)
		return S_ROOM_FULL;
    if(psw != _room->psw)
		return S_ROOM_ILLEGAL_PSW;
    _guest = user;
    _guest->state = PLAYER_STATE_PREPARE; // default state
    return S_OK;
}

STATUS_CODE Room::leave(shared_ptr<player_t> user)
{
	lock_guard<mutex> lock(_guard);
    user->state = PLAYER_STATE_PREPARE; // recover to default state
    if(user == _guest)
    {
		_guest = nullptr;
		_room->guest = "";
		return S_OK;
    }
    if(user == _owner)
    {
		if(_guest)
		{
			_owner = _guest;
			_guest = nullptr;
			_owner->state = PLAYER_STATE_OWNER;
			// so now, the new owner use another chess type.
			_room->owner = _owner->name;
			_room->guest = "";
			return 1;
		}
		else return S_ROOM_EMPTY; // room empty, need to be destroyed
    }
	return S_ROOM_ILLEGAL_OPER; // not in roow
}

STATUS_CODE Room::change_ct(shared_ptr<player_t> user, u32 ct)
{
	if(user != _owner) return S_ROOM_ILLEGAL_OPER;
	if(ct != CHESS_BLACK && ct != CHESS_WHITE) return S_ROOM_ILLEGAL_OPER;
	_room->oct = ct;
	return S_OK;
}

STATUS_CODE Room::change_state(shared_ptr<player_t> user, u32 state)
{
	if(user != _guest) return S_ROOM_ILLEGAL_OPER;
	if(state != PLAYER_STATE_PREPARE && state != PLAYER_STATE_READY)
		return S_ROOM_ILLEGAL_OPER;
	user->state = state;
	return S_OK;
}
