#pragma once
#include "../utils/msg_sock.h"

class UserAgent : public player_t
{
protected:
    shared_ptr<MsgSock> _sock;
    u32 _state;
    u32 _type;
    u32 _chesstype;
    u64 _token;
    shared_ptr<room_t> _room;
protected:
	/* states */
    int idle(shared_ptr<msg_t> msg);
    int room(shared_ptr<msg_t> msg);
    int game(shared_ptr<msg_t> msg);
	
    int rslt(u32 code, const string& msg = "");
public:
	UserAgent(shared_ptr<MsgSock> sock, const string& name);
    virtual ~UserAgent();

    /* 
	 * main loop
	 * handle msgs
	 * util close conn 
	 */
    void mainloop();

    inline u64 token() { return _token; }
	inline void token(u64 t) { _token = t; }
	inline shared_ptr<MsgSock> sock() { return _sock; }
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
