#include "game.h"
#include <stdexcept>
using namespace std;

shared_ptr<Game> Game::_inst = nullptr;

Game::Game()
{}

Game::~Game()
{}

shared_ptr<Game> Game::get()
{
    if(!_inst)
	_inst = shared_ptr<Game>(new Game);
    return _inst;
}

bool Game::name_check(const string& name)
{
    if(name.length <= 0 || name.length >= 36)
	throw runtime_error("name length out of range.");
    // no other rules now
    return true;
}

u32 Game::gen_id()
{
    static u32 ID_GEN = 0;
    return ++ID_GEN;
}

shared_ptr<player_t> Game::register(shared_ptr<player_t> player)
{
    name_check(player->name);
    lock_guard<mutex> lock(_reg_guard);
    auto exist = get_player(player->name);
	if(exist)
	    throw runtime_error("player name conflicts.");
    player->id = gen_id();

    // add to list and map
    player->state = PLAYER_STATE_IDLE;
    _players.emplace_back(player);
    _pmap.put(player->name, player);
    return player;
}

shared_ptr<room_t> Game::create_room(const string& player,
					  shared_ptr<room_t> room)
{
    lock_guard<mutex> lock(_room_guard);
    // TODO check if player joined other room(state)
    auto p = get_player(player);
    if(p->state != PLAYER_STATE_IDLE)
        throw runtime_error("player already joined other room.");
    for(auto p : _rooms)
        if(p->name == room->name)
            throw runtime_error("room name conflicts.");
    
    auto nr = make_shared<Room>(room, p);
    
    // add to list and map
    _rooms.emplace_back(nr);
    _rmap.put(room->name, room);
    return nr;
}

string Game::join_room(const string& player, shared_ptr<room_t> room>)
{
    shared_ptr<Room> target = nullptr;
    for(auto r : _rooms)
        if(room->name == r->name)
        {
            target = room;
            break;
        }
    if(!target)
        throw runtime_error("specified room not exists.");
    
    lock_guard<mutex> lock(target->_guard);
    if(target->_guest) throw runtime_error("room full.");
    if(room->psw != target->psw)
        throw runtime_error("error password.");
    auto p = get_player(player);
    if(!p) throw runtime_error("invalid player.");

    // join player to room, and set state to prepare.
    target->_guest = p;
    p->_state = PLAYER_STATE_PREPARE;

    return target;
}

void Game::exit_room(const string& player, shared_ptr<room_t> room)
{
    shared_ptr<Room> target = get_room(room->name);
    if(!target)
        throw runtime_error("specified room not exists.");
    
    lock_guard<mutex> lock(target->_guard);
    if(target->_guest) throw runtime_error("room full.");
    if(room->psw != target->psw)
        throw runtime_error("error password.");
    auto p = get_player(player);
    if(!p) throw runtime_error("invalid player.");

    // join player to room, and set state to prepare.
    target->_guest = p;
    p->_state = PLAYER_STATE_PREPARE;

    return target;
}

int Game::roomlist(vector<room_t>& rooms)
{}

int Game::change_ct(shared_ptr<room_t> room,
			 const string& player, u32 ct)
{}

void Game::change_state(shared_ptr<room_t> room,
			     const string& player, u32 state)
{}

void Game::start_game(shared_ptr<room_t> room, const string& name)
{}
