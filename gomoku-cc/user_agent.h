#pragma once
#include "../utils/msg_sock.h"
#include <functional>
#include <map>
#include <mutex>
#include <iostream>
#include <atomic>
using namespace std;

// all call back must be msg_result
// then record it with session into a map
// other svr msg, like roomlist, roominfo, just call callbacks in agent directly.
typedef function<void(shared_ptr<msg_result>)> msg_callback;

class UserAgent : public MsgSock
{
protected:
	ostream& _os;
    shared_ptr<player_t> _player;
    shared_ptr<room_t> _room;
    atomic<u32> _state;
    u32 _chesstype;
    u64 _token;

	mutex _sock_guard;

	mutex _session_guard;
	map<u64, msg_callback> _sessions; // 
public:
	UserAgent(ostream& o);
    virtual ~UserAgent();

	inline shared_ptr<player_t> player() { return _player; }
	inline shared_ptr<room_t> room() { return _room; };

public:
	/* callbacks */
	// void on_room_list();
	// void on_room_create();
	// void on_room_join();
	// void on_room_exit();
	// void on_change_state();
	// void on_change_chess();
	// void on_start_match();

	void on_move(int x, int);
	void on_match_end(u64 rslt);
	void on_room_list(const vector<room_t>& rooms);
	void on_room_info(int type, const room_t& room); // join/exit, change ct/st
	void on_match_start(); // match started
public:
	/* player operations */
	void reg(const string& name);
	void unreg();

	void create_room(shared_ptr<room_t>);
	void join_room(shared_ptr<room_t>);
	void exit_room();
	void change_chess();
	void change_state();
	void start_match();
	void move(int x,  int y);


	/* handle msg from svr. */
	void msg_proc();
	/* handle msg */
	void on_msg(shared_ptr<msg_t>);
};
