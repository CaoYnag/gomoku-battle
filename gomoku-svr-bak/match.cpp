#include "game.h"

Game::Game(shared_ptr<Room> room) : _room(room), _state(GAME_STATE_WAITING)
{
    // NOTICE white go first.
    if(room->_owner_chess = CHESS_WHITE)
    {
	_player[0] = room->_owner;
	_player[1] = room->_guest;
    }
    else
    {
	_player[0] = room->_guest;
	_player[1] = room->_owner;
    }
}

Game::~Game()
{
    // nothing to do.
}


void Game::start()
{
    if(_state != GAME_STATE_WAITING) return;
    _state = GAME_STATE_RUNNING;
    _player[0]->game_start();
    _player[1]->game_start();
    _gmk.start();
}

void Game::mainloop()
{
    int cp = 0;
    int ct = CHESS_WHITE;
    int cts = CHESS_WHITE + CHESS_BLACK;
    shared_ptr<msg_move> mv = nullptr;
    int err_cnt = 0;
    while(_state == GAME_STATE_RUNNING)
    {
	if(err_cnt >= MAX_ERR_CNT) error("too much illegal oper.");
	mv = static_pointer_cast<msg_move>(_player[cp]->next_oper());
	if(!mv)
	{
	    ++err_cnt;
	    continue;
	}
	if(_gmk.put(mv->x, mv->y, cp))
	{
	    ++err_cnt;
	    continue;
	}

	// check game status.
	if(_gmk.end())
	{
	    end(ct == CHESS_BLACK);
	    break;
	}
	
	// switch player.
	err_cnt = 0;
	mv->chess = ct;
	cp = 1 - cp;
	ct = cts - ct;
	_player[cp]->prev_move(mv);
    }
}

void Game::end(int p)
{
    _state = GAME_STATE_END;
    if(p < 0 || p > 1) // draw
    {
	_player[0]->game_draw();
	_player[1]->game_draw();
    }
    else
    {
	_player[p]->game_win();
	_player[1 - p]->game_lose();
    }
}

void Game::error(const string& msg)
{
    _state = GAME_STATE_END;
    _player[0]->game_error(msg);
    _player[1]->game_error(msg);
}

void Game::run()
{
    start();
    mainloop();
}
