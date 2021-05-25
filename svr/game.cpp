#include "game.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
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
    if(name.length() <= 0 || name.length() >= 36)
	throw runtime_error("name length out of range.");
    // no other rules now
    return true;
}

bool Game::room_check(shared_ptr<room_t> room)
{
    if(!room) throw runtime_error("empty room info.");
    if(room->name.length() <= 0 || room->name.length() >= 36)
	throw runtime_error("name length out of range.");
    if(room->psw.length() >= 36)
	throw runtime_error("psw length out of range.");
    
    return true;
}

u32 Game::gen_id()
{
    static u32 ID_GEN = 0;
    return ++ID_GEN;
}

shared_ptr<player_t> Game::register_player(shared_ptr<player_t> player)
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
    // _pmap.emplace(player->name, player);
    _pmap[player->name] = player;
    return player;
}

shared_ptr<room_t> Game::create_room(const string& player,
					  shared_ptr<room_t> room)
{
    room_check(room);
    lock_guard<mutex> lock(_room_guard);
    // TODO check if player joined other room(state)
    auto p = get_player(player);
    if(!p) throw runtime_error("player not exists.");
    if(p->state != PLAYER_STATE_IDLE)
        throw runtime_error("player already joined other room.");
    for(auto p : _rooms)
        if(p->name == room->name)
            throw runtime_error("room name conflicts.");
    
    auto nr = make_shared<Room>(room, p);
    
    // add to list and map
    _rooms.emplace_back(nr);
    // _rmap.emplace(room->name, static_pointer_cast<Room>(room));
    _rmap[room->name] = nr;
    return nr;
}

shared_ptr<room_t> Game::join_room(const string& player, shared_ptr<room_t> room)
{
    room_check(room);
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
    target->state = ROOM_STATE_FULL;
    target->_guest = p;
    p->state = PLAYER_STATE_PREPARE;

    return target;
}

void Game::exit_room(const string& player, const string& room)
{
    auto r = get_room(room);
    auto p = get_player(player);
    if(!r)
        throw runtime_error("specified room not exists.");
    lock_guard<mutex> lock(r->_guard);
    if(!p || (p != r->_owner && p != r->_guest))
	throw runtime_error("player not exists or not in this room.");

    // if guest, just exit.
    if(p == r->_guest)
    {
	p->state = PLAYER_STATE_IDLE;
	r->_guest = nullptr;
	r->state = ROOM_STATE_OPEN;
	return;
    }

    // if is owner
    // if has guest
    if(r->_guest)
    {
	p->state = PLAYER_STATE_IDLE;
	p = r->_guest;
	r->_guest = nullptr;
	p->state = PLAYER_STATE_OWNER;
	r->state = ROOM_STATE_OPEN;
	r->_owner = p;
	return;
    }
    // no guest, destroy room.
    lock_guard<mutex> lock_all(_room_guard);
    p->state = PLAYER_STATE_IDLE;
    erase(_rooms, r);
    _rmap.erase(room);
}

void Game::roomlist(vector<shared_ptr<room_t>>& rooms)
{
    rooms.clear();
    for(auto p : _rooms)
	rooms.emplace_back(static_pointer_cast<room_t>(p));
}

int Game::change_ct(const string& room,
			 const string& player, u32 ct)
{
    auto r = get_room(room);
    auto p = get_player(player);
    if(!r)
	throw runtime_error("room not exists.");
    if(!p || p != r->_owner)
    	throw runtime_error("player not exists or not room owner.");    
    if(ct != CHESS_BLACK && ct != CHESS_WHITE)
	throw runtime_error("invalid chesstype.");

    lock_guard<mutex> lock(r->_guard);
    if(ct == r->oct) return S_NOCHANGE;
    r->oct = ct;
    return S_OK;
}

int Game::change_state(const string& room,
			     const string& player, u32 state)
{
    auto r = get_room(room);
    auto p = get_player(player);
    if(!r)
	throw runtime_error("room not exists.");
    if(!p || p != r->_guest)
    	throw runtime_error("player not exists or not guest.");    
    if(state != PLAYER_STATE_PREPARE && state != PLAYER_STATE_READY)
	throw runtime_error("invalid state.");

    lock_guard<mutex> lock(r->_guard);
    if(state == p->state) return S_NOCHANGE;
    p->state = state;
    return S_OK;
}

shared_ptr<Match> Game::start_match(const string& room, const string& player)
{
    auto r = get_room(room);
    auto p = get_player(player);
    if(!r)
	throw runtime_error("room not exists.");
    if(!p || p != r->_owner)
    	throw runtime_error("player not exists or not room owner.");    
    if(!r->_guest || r->_guest->state != PLAYER_STATE_READY)
	throw runtime_error("guest not exists or not ready.");
    lock_guard<mutex> lock(r->_guard);
    r->state = ROOM_STATE_MATCH;
    return make_shared<Match>(r);
}

void Game::match_ovr(shared_ptr<Match> m)
{
    if(m->end())
    {
	auto r = m->room();
	lock_guard<mutex> lock(r->_guard);
	r->state = ROOM_STATE_FULL;
	r->_guest->state = PLAYER_STATE_PREPARE;

	// TODO record stats in future.
    }
}
