#pragma once
#include "room.h"
#include <vector>
#include <map>
#include <thread>
#include <memory>
using namespace std;

class Game
{
protected:
    vector<shared_ptr<Room>> _rooms;            // all rooms
    map<string, shared_ptr<Room>> _rmap;
    vector<shared_ptr<player_t>> _players;      // all players
    map<string, shared_ptr<player_t>> _pmap;    // name : player
    mutex _player_guard;  // player guard
    mutex _room_guard; // room guard

public:
    virtual ~Game();

protected:
    /* sync operations, make sure lock is acquired */
    void add_player(shared_ptr<player_t> player);
    void rm_player(shared_ptr<player_t> player);
    void add_room(shared_ptr<room_t> room);
    void rm_room(const string& name);

public:
    /*
     * check name format.
     * return true for success
     * throw an exception for failed msg.
     */
    bool name_check(const string& name);

    /*
     * generate id for player.
     */
    u32 gen_id();

    /* interfaces */
public:
    /*
       user intfs
       * if succ, return the player pointer.
       * if failed, return nullptr.
    */
    shared_ptr<player_t> reg(shared_ptr<player_t> player);

    /* room intfs */
    /*
     * create a room.
     * if succ, return created room pointer.
     * if fail, throw an exception with error msg
     */
    shared_ptr<room_t> create_room(const string& player, shared_ptr<room_t> room);

    /*
    * player join a room, if succ, return owner name.
    * if fail, throw an exception.
    * and if join succ, svr should notice room owner.
    **/
    string join_room(const string &player, shared_ptr<room_t> room);

    /*
     * player exit room.
     * if fail, throw an exception.
     * if succ, notice remain player. if no player remain, destroy room.
     */
    void exit_room(const string &player, shared_ptr<room_t> room);

    /* get room list */
    int roomlist(vector<room_t>& rooms);

    /* change chess type */
    int change_ct(shared_ptr<room_t> room, const string& player, u32 ct);

    /* change state */
    void change_state(shared_ptr<room_t> room, const string& player, u32 state);

    /* game operations */
    /*
     * start game
     * if succ, start a game thread.
     * if fail, throw an exception with failed msg.
     * failure situation:
     *   user not room owner
     *   player not enough or ready.
     */
    void start_game(shared_ptr<room_t> room, const string& name);

private:
    Game();

    Game(const Game &) = delete;

    static shared_ptr <Game> _inst;
public:
    static shared_ptr <Game> get();
};
