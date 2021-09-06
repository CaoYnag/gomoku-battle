#pragma once
#include "msg.h"
#include "net_utils.h"
#include <mutex>
using namespace std;
/**
MsgSock applys msg snd/rcv abbilty.

and use token() and session() to set up or invalidate token and session.
*/
class MsgSock : public TcpSock
{
protected:
    static int MAX_ILLEGAL_OPER;
    
    string _tmp;
    int _failures;
	mutex _fguard;
    
    const char PACKET_BEGIN_CHAR = '<';
    const char PACKET_END_CHAR = '>';

	u64 _token;
	u64 _session;
public:
    MsgSock();
    /* bugs when  */
    MsgSock(shared_ptr<TcpSock> ptr);
    MsgSock(int port);
    MsgSock(const string& ip, int port);
    MsgSock(SOCK sock, const sockaddr_in& addr);
	MsgSock(const MsgSock&) = delete;
    virtual ~MsgSock();

    shared_ptr<MsgSock> accept_msg_sock();

    int fail();

    static void max_failures(int v);

	inline void session(u64 s = INVALID_SESSION) { _session = s; }
	inline void token(u64 t = INVALID_TOKEN) { _token = t; }
public:
    template<class T>
    int send_msg(T&& msg)
    {
		msg.token = _token;
		msg.session = _session;
        auto raw = pack(msg);
        if(raw.empty()) return 1;
        raw = PACKET_BEGIN_CHAR + raw + PACKET_END_CHAR;
        return send(raw);
    }
    shared_ptr<msg_t> rcv_msg();

    /* msg snd intfs */
    int rslt(u32 stat, const string& rslt = "");
    int req_rooms();
    int req(u32 target, u32 oper);
    int reg(const string& name);
    int roomlist(const vector<room_t>& rooms);
    int create_room(const string& name, const string& psw);
    int join_room(const string& name, const string& psw);
    int exit_room(const string& name);
    int roominfo(u32 type, const room_t& room);
    int choose_chess(u32 type);
    int set_chess(u32 type);
    int snd_chess(u32 type);
    int user_state(u32 state);
	int start_game();
	int game_start();
    int game_win();
    int game_lose();
    int game_draw();
	int game_err();
    int snd_game(u32 state, u32 ex);
    int move(u32 x, u32 y);
    int move(u32 chess, u32 x, u32 y);
};
