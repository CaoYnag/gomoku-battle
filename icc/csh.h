#pragma once
#include "cmds.h"
#include "user_agent.h"
#include "sync_io.h"
#include <functional>
using namespace std;

/*

*/
class Csh
{
private:
	vector<function<void(shared_ptr<cmd_t>)>> _executors;
	shared_ptr<SyncIO> _io;
	shared_ptr<UserAgent> _ua;
	string _pfmt; // prompt format
	bool _running;

	string prompt_str();
	void execute(shared_ptr<cmd_t>);
	void parse_and_execute(const string&);

	void release();
private:
	/* execute cmds */
	void execute_help(shared_ptr<cmd_t>);
	void execute_exit(shared_ptr<cmd_t>);
	void execute_stat(shared_ptr<cmd_t>);
	void execute_ping(shared_ptr<cmd_t>);
	void execute_connect(shared_ptr<cmd_t>);
	void execute_reg(shared_ptr<cmd_t>);
	void execute_unreg(shared_ptr<cmd_t>);
	void execute_room_list(shared_ptr<cmd_t>);
	void execute_create_room(shared_ptr<cmd_t>);
	void execute_join_room(shared_ptr<cmd_t>);
	void execute_exit_room(shared_ptr<cmd_t>);
	void execute_ct(shared_ptr<cmd_t>);
	void execute_state(shared_ptr<cmd_t>);
	void execute_match(shared_ptr<cmd_t>);
	void execute_move(shared_ptr<cmd_t>);
	void execute_board(shared_ptr<cmd_t>);
public:
	Csh();
	~Csh();

	void run();
	void close();
};
