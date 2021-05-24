#pragma once
#include "user_agent.h"
#include "../utils/gomoku.h"
#include "funds.h"

class Game
{
protected:
    Gomoku _gmk;
    int _state;
    shared_ptr<Room> _room;
    shared_ptr<UserAgent> _player[2];

    /* env prepare. */
    void start();
    /* main game */
    void mainloop();
    /* error ocurr and game interrupt. */
    void error(const string& msg);
    /* game end with win player. if -1 offered, game draw. */
    void end(int p);
public:
    Game(shared_ptr<Room> room);
    virtual ~Game();

    /*
     * run the game, util game end or some fatal error occur.
     */
    void run();
};
