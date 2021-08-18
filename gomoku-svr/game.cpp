#include "game.h"
#include <stdexcept>

using namespace std;

shared_ptr<Game> Game::_inst = nullptr;

Game::Game() {}

Game::~Game()
{}

shared_ptr<Game> Game::get()
{
    if (!_inst)
        _inst = shared_ptr<Game>(new Game);
    return _inst;
}

void Game::add_player(shared_ptr<player_t> player)
{
    // TODO search first. make sure no collide data.
	player->id = gen_id();
    _players.push_back(player);
    _pmap[player->name] = player;
}

void Game::rm_player(shared_ptr<player_t> player)
{
    // TODO complete this later.
}

void Game::add_room(shared_ptr<Room> room)
{
    _rooms.push_back(room);
    _rmap[room->name] = room;
}
void Game::rm_room(const string& name)
{
    // TODO complete this later.
}

bool Game::name_check(const string &name)
{
    if (name.length <= 0 || name.length >= 36)
        throw runtime_error("name length out of range.");
    // no other rules now
    return true;
}

u32 Game::gen_id()
{
    static u32 ID_GEN = 0;
    return ++ID_GEN;
}

shared_ptr<player_t> Game::reg(shared_ptr<player_t> player)
{
    name_check(player->name);
    lock_guard<mutex> lock(_player_guard);
    if(_pmap.count(player->name))
        throw runtime_error("player name conflicts.");
    player->id = gen_id();
    add_player(player);
    return player;
}

shared_ptr<room_t> Game::create_room(const string& player, shared_ptr<room_t> room)
{
    // same name check here?
	{
		lock_guard<mutex> lock(_player_guard);
		if(!_pmap.count(player))
			throw runtime_error("invalid player.");
	}
    lock_guard<mutex> lock(_room_guard);
    if(_rmap.count(room->name))
        throw runtime_error("room already exists.");

    add_room(make_shared<Room>(room, _pmap[player]));
    return room;
}

string Game::join_room(const string& player, shared_ptr<room_t> room)
{
	{
		lock_guard<mutex> lock(_player_guard);
		if(_pmap.count(player))
			throw runtime_error("invalid player.");
	}
	// TODO maybe need read/write locks.
	auto msg = _rmap[room->name]->join(_pmap[player]);
	if(!msg.empty())
	{
		// join failed
		throw runtime_error(msg);
	}
	return _rmap[room->name];
}

void Game::exit_room(const string& player, shared_ptr<room_t> room)
{
	// TODO make sure room and player exists.
	auto r = _rmap[room->name];
	if(r->leave(_pmap[player]))
	{
		// need destroy room.
		rm_room(room->name);
	}
}

int Game::roomlist(vector<room_t>& rooms)
{
	lock_guard<mutex> lock(_room_guard);
	for(auto pr : _rooms)
		rooms.push_back(*static_pointer_cast<room_t>(pr));

	return rooms.size();
}

int Game::change_ct(shared_ptr<room_t> room, const string& player, u32 ct)
{
	// TODO complete this later.
	return 0;
}

void Game::change_state(shared_ptr<room_t> room,
                        const string& player, u32 state)
{
	// TODO complete this later.
}

void Game::start_game(shared_ptr<room_t> room, const string& name)
{
	// TODO complete this later.
}
