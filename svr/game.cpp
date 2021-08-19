#include "game.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <algorithm>
using namespace std;

Game::Game()
{}

Game::~Game()
{}

void Game::add_player(shared_ptr<player_t> player)
{
	player->id = gen_id();
	player->state = PLAYER_STATE_IDLE;
    _players.push_back(player);
    _pmap[player->name] = player;
}

void Game::rm_player(const string& name)
{
    // TODO complete this later.
	auto it = find_if(_players.begin(), _players.end(),
					  [&](const shared_ptr<player_t>& p){ return p->name == name; });
	if(it != _players.end())
		_players.erase(it);
	_pmap.erase(name);
}

void Game::add_room(shared_ptr<Room> room)
{
    _rooms.push_back(room);
    _rmap[room->_room->name] = room;
}
void Game::rm_room(const string& name)
{
	auto it = find_if(_rooms.begin(), _rooms.end(),
					  [&](const shared_ptr<Room>& p){ return p->_room->name == name; });
	if(it != _rooms.end())
		_rooms.erase(it);
	_rmap.erase(name);
}

bool Game::player_check(shared_ptr<player_t> player)
{
    if(player->name.length() <= 0 || player->name.length() >= 36)
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


STATUS_CODE Game::register_player(shared_ptr<player_t> player)
{
    try{player_check(player);}
	catch(exception& e)
	{
		spdlog::warn("error register player {}: {}", player->name, e.what());
		return S_PLAYER_INVALID_META;
	}
    lock_guard<mutex> lock(_reg_guard);
    auto exist = get_player(player->name);
	if(exist)
	{
		spdlog::warn("register failed, player exists: {}", player->name);
		return S_PLAYER_EXISTS;
	}
    add_player(player);
    return S_OK;
}

STATUS_CODE Game::unregister_player(shared_ptr<player_t> player)
{
	lock_guard<mutex> lock(_reg_guard);
	if(_pmap.count(player->name))
	{
		player = _pmap[player->name];
		if(player->state != PLAYER_STATE_IDLE)
			return S_PLAYER_BUSY;
		rm_player(player->name);
		return S_OK;
	}
	return S_PLAYER_INVALID;
}

STATUS_CODE Game::create_room(const string& player,
					  shared_ptr<room_t> room)
{
    try{room_check(room);}
	catch(exception& e)
	{
		spdlog::debug("error create room {} by {}: {}", room->name, player, e.what());
		return S_ROOM_INVALID_META;
	}

	// TODO need lock this player?
    auto p = get_player(player);
    if(!p)
	{
		spdlog::debug("error create room {}: player {} not exists.", room->name, player);
		return S_PLAYER_INVALID;
	}
    if(p->state != PLAYER_STATE_IDLE)
	{
		spdlog::debug("player {} already joined other room.", player);
		return S_PLAYER_BUSY;
	}
	
	lock_guard<mutex> lock(_room_guard);
    if(_rmap.count(room->name))
	{
		spdlog::debug("error create room {} by {}: room already eists.", room->name, player);
		return S_ROOM_EXISTS;
	}
    auto nr = make_shared<Room>(room, p);
    add_room(nr);
    return S_OK;
}

STATUS_CODE Game::join_room(const string& player, shared_ptr<room_t> room)
{
    shared_ptr<Room> target = get_room(room->name);
    if(!target)
	{
		spdlog::debug("{} join {} failed: room not exists.", player, room->name);
		return S_ROOM_NOT_EXISTS;
	}
	auto p = get_player(player);
	if(!p)
	{
		spdlog::debug("error join room {}: player {} not exists.", room->name, player);
		return S_PLAYER_INVALID;
	}
    if(p->state != PLAYER_STATE_IDLE)
	{
		spdlog::debug("player {} already joined other room.", player);
		return S_PLAYER_BUSY;
	}
    lock_guard<mutex> lock(target->_guard);
	return target->join(p, room->psw);
}

STATUS_CODE Game::exit_room(const string& player, const string& room)
{
    auto r = get_room(room);
    auto p = get_player(player);
    if(!r)
    {
		spdlog::debug("error {} leaving {}: room not exists");
		return S_ROOM_NOT_EXISTS;
	}
	if(!p)
	{
		spdlog::debug("error exit room {}: player {} not exists.", r->_room->name, player);
		return S_PLAYER_INVALID;
	}
    lock_guard<mutex> lock(r->_guard);
	auto ret = r->leave(p);
	if(ret == S_ROOM_EMPTY)
	{
		rm_room(r->_room->name);
		return S_OK;
	}
	return ret;
}

void Game::roomlist(vector<room_t>& rooms)
{
	lock_guard<mutex> lock(_room_guard);
    rooms.clear();
    for(auto p : _rooms)
		rooms.emplace_back(*(p->_room));
}

STATUS_CODE Game::change_ct(const string& room,
			 const string& player, u32 ct)
{
    auto r = get_room(room);
	auto p = get_player(player);
    if(!r)
	{
		spdlog::debug("{} change state in {} failed: room not exists.", player, room);
		return S_ROOM_NOT_EXISTS;
	}
	
	return r->change_ct(p, ct);
}

STATUS_CODE Game::change_state(const string& room,
			     const string& player, u32 state)
{
    auto r = get_room(room);
    auto p = get_player(player);
    if(!r)
	{
		spdlog::debug("{} change state in {} failed: room not exists.", player, room);
		return S_ROOM_NOT_EXISTS;
	}
	
	return r->change_state(p, state);
}

STATUS_CODE Game::start_match(const string& room, const string& player)
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
    r->_room->state = ROOM_STATE_MATCH;
    //return make_shared<Match>(r);
	return S_OK;
}

void Game::match_ovr(shared_ptr<Match> m)
{
    if(m->end())
    {
	auto r = m->room();
	lock_guard<mutex> lock(r->_guard);
	r->_room->state = ROOM_STATE_FULL;
	r->_guest->state = PLAYER_STATE_PREPARE;

	// TODO record stats in future.
    }
}
