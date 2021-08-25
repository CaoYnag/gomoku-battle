#pragma once
#include "../svr/player.h"
#include "../utils/msg_sock.h"

class UserAgent : public Player
{
protected:
    shared_ptr<MsgSock> _sock;
    u32 _state;
    u32 _type;
    u32 _chesstype;
    u64 _token;
    shared_ptr<room_t> _room;

	mutex _mg;
	vector<shared_ptr<msg_move>> _moves;
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
    void game_error();
    void prev_move(shared_ptr<msg_move> msg);
public:
    /* some sync oper */
    shared_ptr<msg_t> next_oper();
public:
	/* Player Intfs */
	int next_move(u64 mid, int& x, int& y);
	void enemy_move(int x, int y);
	void game_result(shared_ptr<match_result> rslt);
	void notice(u32 stat);
};
