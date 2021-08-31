#include "game.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <algorithm>
using namespace std;

Game::Game()
{}

Game::~Game()
{}

void Game::add_player(shared_ptr<Player> player)
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
					  [&](const shared_ptr<Player>& p){ return p->name == name; });
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

bool Game::player_check(shared_ptr<Player> player)
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


STATUS_CODE Game::register_player(shared_ptr<Player> player)
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
	spdlog::info("registered player: {}", player->name);
    return S_OK;
}

STATUS_CODE Game::unregister_player(shared_ptr<Player> player)
{
	lock_guard<mutex> lock(_reg_guard);
	if(_pmap.count(player->name))
	{
		player = _pmap[player->name];
		if(player->state != PLAYER_STATE_IDLE)
			return S_PLAYER_BUSY;
		rm_player(player->name);
		spdlog::info("player {} unregistered.", player->name);
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
	spdlog::info("player {} created room {} with psw {}", player, room->name, room->psw);
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
	{
		spdlog::debug("{} start game in room {} failed: room not exists.", player, room);
		return S_ROOM_NOT_EXISTS;
			}
    if(p != r->_owner)
	{
		spdlog::debug("{} start game in room {} failed: player not exists.", player, room);
		return S_PLAYER_INVALID;
	}
	if(p != r->_owner)
	{
		spdlog::debug("{} start game in room {} failed: player not room owner.", player, room);
		return S_ROOM_ILLEGAL_OPER;
	}
    if(!r->_guest || r->_guest->state != PLAYER_STATE_READY)
	{
		spdlog::debug("start game in room {} failed: guest not exists or not ready.", player);
		return S_ROOM_ILLEGAL_OPER;
	}

    lock_guard<mutex> lock(r->_guard); // TODO should not room's lock in Game.
    r->_room->state = ROOM_STATE_MATCH;
    auto m = make_shared<Match>(r);
	if(!m)
	{
		spdlog::debug("start match in {} failed.", room);
		return S_ERROR;
	}
	_mmap[room] = m;
	return S_OK;
}

void Game::match_ovr(shared_ptr<Match> m, shared_ptr<match_result> result)
{
    if(result)
	{
		// update stats here
		switch(result->result)
		{
		case MATCH_RESULT_DRAW:
		{
			m->room()->_owner->stats()->game_draw(result->steps);
			m->room()->_guest->stats()->game_draw(result->steps);
		}break;
		case MATCH_RESULT_OWNER_WIN:
		{
			m->room()->_owner->stats()->game_win(result->steps);
			m->room()->_guest->stats()->game_lose(result->steps);
		}break;
		case MATCH_RESULT_GUEST_WIN:
		{
			m->room()->_owner->stats()->game_lose(result->steps);
			m->room()->_guest->stats()->game_win(result->steps);
		}break;
		default: // error
		{}break;
		}
	}

	// remove from mmap
	_mmap.erase(m->room()->_room->name);
	// recover room and player state?
	auto r = m->room();
	r->change_state(r->_guest, PLAYER_STATE_PREPARE);
	r->_room->state = ROOM_STATE_FULL;
}
