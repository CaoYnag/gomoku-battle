#pragma once
#include <atomic>
#include <map>
#include <vector>
#include "../utils/msg_sock.h"
#include "../svr/game.h"
#include "user_agent.h"
#include "match_svr.h"
using namespace std;

/**
GameSvr only contains intfs communicated with clients.
All game logic implemented in Game.

when rcv a new connection, game svr try rcv a `reg msg.
when rcv the reg msg, svr generate a token for it, or find associated user_agent..

this token will used for reconnection in future.

each msg snd to svr should bring a `session` field, then svr reply clients will set same session.
clients can use session field to identify different response for his requests.
*/
class GameSvr
{
protected:
	shared_ptr<MatchRunner> _runner;
	shared_ptr<Game> _game;
	shared_ptr<MsgSock> _svr;
	int _conn;
	map<string, shared_ptr<UserAgent>> _conns; // player name : connection
	map<u64, string> _tokens; // token : player name
	atomic<int> _running;
public:
	virtual ~GameSvr();

	int init();
	void service();
	void shutdown();
protected:
	STATUS_CODE reg(shared_ptr<MsgSock> sock, shared_ptr<msg_reg> msg);
	void release_conn(shared_ptr<MsgSock> sock);

	/* handle new connection */
	void handle_new_conn(shared_ptr<MsgSock> sock);

	inline shared_ptr<UserAgent> get_agent(const string& name) { return _conns[name]; }
private:
	GameSvr();
	GameSvr(const GameSvr&) = delete;
	static shared_ptr<GameSvr> _inst;
public:
	static shared_ptr<GameSvr> get();

	inline shared_ptr<Game> game() { return _game; }

public:
	/* game intfs */
	STATUS_CODE register_player(shared_ptr<Player> player);
	STATUS_CODE unregister_player(shared_ptr<Player> player);
    STATUS_CODE create_room(const string& player, shared_ptr<room_t> room);
    STATUS_CODE join_room(const string& player, shared_ptr<room_t> room);
    STATUS_CODE exit_room(const string& player, const string& room);
    void roomlist(vector<room_t>&);
    STATUS_CODE change_ct(const string& room, const string& player, u32 ct);
    STATUS_CODE change_state(const string& room, const string& player, u32 state);
    STATUS_CODE start_match(const string& room, const string& player);
	void match_end(shared_ptr<Match> m, shared_ptr<match_result> r);
};
