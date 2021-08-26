#pragma once
#include "../utils/msg_sock.h"
#include <functional>
#include <map>
#include <mutex>
using namespace std;

typedef function<void()> msg_callback;

class UserAgent : public MsgSock
{
protected:
    shared_ptr<player_t> _player;
    shared_ptr<room_t> _room;
    u32 _state;
    u32 _chesstype;
    u64 _token;

	mutex _session_guard;
	map<u64, msg_callback> _sessions;
public:
	UserAgent();
    virtual ~UserAgent();

	inline shared_ptr<player_t> player() { return _player; }
	inline shared_ptr<room_t> room() { return _room; };
public:
	/* player operations */
	shared_ptr<player_t> reg(const string& name);
	u32 unreg();

	u32 create_room(shared_ptr<room_t>);
	u32 join_room(shared_ptr<room_t>);
	u32 exit_room();
	u32 change_chess(u32 type);
	u32 change_state(u32 state);
	u32 start_match();
	u32 move(int x,  int y);


	/* handle msg from svr. */
	void msg_proc();
	/* handle msg */
	void on_msg(shared_ptr<msg_t>);
};
