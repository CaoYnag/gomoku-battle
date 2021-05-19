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
    lock_guard<mutex> lock(_guard);
    for(auto p : _players)
	if(p->name == player->name)
	    throw runtime_error("player name conflicts.");
    player->id = gen_id();
    return player;
}

shared_ptr<room_t> Game::create_room(const string& player,
					  shared_ptr<room_t> room)
{
    
}

string Game::join_room(const string& player, shared_ptr<room_t> room>)
{}

void Game::exit_room(const string& player, shared_ptr<room_t> room)
{}

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
