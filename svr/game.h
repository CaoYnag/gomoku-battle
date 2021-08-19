#pragma once
#include "room.h"
#include <vector>
#include <map>
#include <thread>
#include <memory>
#include "match.h"
using namespace std;

/**
Game main logic

this class contains all game logic.
include player/room/game operations.

*NOTICE*: never create player_t, room_t inst in this class.

1. Player
a player name was limited in 36 characters. and some other restrictions.
Game won't do additional check to identify player. it just use name to identify each player.
Additional check should be done in GameSvr, which wraps this class.
intfs:
reg
unreg

2. Room

*/
class Game
{
protected:
    vector<shared_ptr<Room>> _rooms;            // all rooms
    map<string, shared_ptr<Room>> _rmap;
    vector<shared_ptr<player_t>> _players;      // all players
    map<string, shared_ptr<player_t>> _pmap;    // name : player
    mutex _reg_guard;  // reg guard
    mutex _room_guard; // room guard

public:
    virtual ~Game();
protected:
	/* 
	 * some helper functions 
	 * all functions will modify datas directly.
	 * so, make sure guard locked and data legally.
	 */
	
	void add_player(shared_ptr<player_t> player);
	void rm_player(const string& name);
	void add_room(shared_ptr<Room> room);
	void rm_room(const string& name);
public:
    /* some data intf */
    // shared_ptr<Player> get_player(u32 id);
    inline shared_ptr<player_t> get_player(const string& name)
    {
        return _pmap[name];
    } 
    inline shared_ptr<Room> get_room(const string& name)
    {
        return _rmap[name];
    }
public:
    /* some monitor intf here */
public:
    /*
	 * check player metadata
	 * name only now
     * check name format.
     * return true for success
     * throw an exception for failed msg.
     */
    bool player_check(shared_ptr<player_t> player);
    /*
     * check room name and psw format.
     */
    bool room_check(shared_ptr<room_t> room);
    /*
     * generate id for player.
     */
    u32 gen_id();
    
    /* interfaces */
public:
    /* 
	 * S_PLAYER_INVALID_META for invalid player info
	 * S_PLAYER_EXISTS for player already exists.
	 */ 
	STATUS_CODE register_player(shared_ptr<player_t> player);
	/*
	 * S_PLAYER_INVALID
	 * S_PLAYER_BUSY
	 */
	STATUS_CODE unregister_player(shared_ptr<player_t> player);
	/*
	 * S_ROOM_INVALID_META
	 * S_ROOM_EXISTS
	 * S_PLAYER_INVALID
	 * S_PLAYER_BUSY
	 */
    STATUS_CODE create_room(const string& player, shared_ptr<room_t> room);
    /* 
	 * S_ROOM_NOT_EXISTS
	 * S_ROOM_FULL
	 * S_ROOM_ILLEGAL_ACCESS
	 * S_ROOM_ALREAY_INSIDE
	 * S_PLAYER_INVALID
	 * S_PLAYER_BUSY
	 */
    STATUS_CODE join_room(const string& player, shared_ptr<room_t> room);
    /*
	 * S_PLAYER_NOT_EXISTS
	 * S_ROOM_NOT_EXISTS
	 * S_ROOM_ILLEGAL_OPER for not in room
	 * if room empty after player exit, destroy this room
     */
    STATUS_CODE exit_room(const string& player, const string& room);
    /*
     * get room list
     */
    void roomlist(vector<room_t>&);
    /*
	 * S_ROOM_ILLEGAL_OPER for not owner, or invalid ct
     */
    STATUS_CODE change_ct(const string& room, const string& player, u32 ct);
    /* 
	 * S_ROOM_ILLEGAL_OPER for not guest, or invalid state
     */
    STATUS_CODE change_state(const string& room, const string& player, u32 state);

    /* 
	 * S_GAME_NO_GUEST
	 * S_GAME_ILLEGAL_OPER not owner
     */
	// TODO how can i emit a match?
    STATUS_CODE start_match(const string& room, const string& player);

    /*
     * callback when a match done.
     */
    void match_ovr(shared_ptr<Match> m);
public:
    Game();
    Game(const Game&) = delete;
};
