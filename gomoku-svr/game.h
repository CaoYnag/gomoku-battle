#pragma once
#include "../utils/msg_sock.h"
#include "../utils/gomoku.h"

constexpr const int MAX_ERR_CNT = 5;

class UserAgent : public player_t
{
protected:
    shared_ptr<MsgSock> _sock;
    u32 _state;
    u32 _type;
    u32 _chesstype;
    shared_ptr<room_t> _room;
protected:
    int idle(shared_ptr<msg_t> msg);
    int room(shared_ptr<msg_t> msg);
    int game(shared_ptr<msg_t> msg);

    int fail(u32 code, const string& msg);
public:
    UserAgent(shared_ptr<MsgSock> sock, u32 d, const string& name);
    virtual ~UserAgent();

    /* main loop, util close conn */
    void mainloop();

public:
    /* some callbacks */
    /* set player chess type */
    void changechess(u32 type);
    /* user join/exit */
    void roominfo(u32 type, const string& name);
    /* game infos */
    void game_start();
    void game_win();
    void game_lose();
    void game_draw();
    void game_error(const string& msg);
    void prev_move(shared_ptr<msg_move> msg);
public:
    /* some sync oper */
    shared_ptr<msg_t> next_oper();
};

class Room : public room_t
{
public:
    shared_ptr<UserAgent> _owner;
    u32 _owner_chess;
    shared_ptr<UserAgent> _guest;
public:
    Room(u32 i, const string& n, const string& p, u32 s,
	 shared_ptr<UserAgent> owner);
    ~Room();

    /*
     * empty string for join succ.
     * or msg why join failed.
     */
    string join(shared_ptr<UserAgent> user, const string& psw);
    /* 
     * return players remain.
     * so if `0` is returned, this room need to be destroy
     */
    u32 leave(shared_ptr<UserAgent> user);
    
};


/*
 * a match.
 */
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
