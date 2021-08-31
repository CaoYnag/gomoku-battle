#include "csh.h"
#include "cmd_io.h"
#include <sstream>
#include <thread>
using namespace std;
using namespace std::placeholders;

Csh::Csh() : _io(make_shared<SyncIO>()), _ua(make_shared<UserAgent>(_io)), _running(true),
			 _executors(
				 {
					 bind(&Csh::execute_help, this, _1),
					 bind(&Csh::execute_exit, this, _1),
					 bind(&Csh::execute_stat, this, _1),
					 bind(&Csh::execute_ping, this, _1),
					 bind(&Csh::execute_connect, this, _1),
					 bind(&Csh::execute_reg, this, _1),
					 bind(&Csh::execute_unreg, this, _1),
					 bind(&Csh::execute_room_list, this, _1),
					 bind(&Csh::execute_create_room, this, _1),
					 bind(&Csh::execute_join_room, this, _1),
					 bind(&Csh::execute_exit_room, this, _1),
					 bind(&Csh::execute_ct, this, _1),
					 bind(&Csh::execute_state, this, _1),
					 bind(&Csh::execute_match, this, _1),
					 bind(&Csh::execute_move, this, _1),
					 bind(&Csh::execute_board, this, _1)
				 })
{
}

Csh::~Csh()
{
	release();
}

void Csh::release()
{
	if(_ua) // release useragent
	{
		_ua->close();
		_ua = nullptr;
	}
}

void Csh::close()
{
	_io->println("Bye.");
	_running = false;
	release();
}

string Csh::prompt_str()
{
	// TODO load prompt fmt from conf in ctor, then format here.
	return "$";
}

void Csh::execute(shared_ptr<cmd_t> cmd)
{
	if(!cmd)
	{
		_io->errorln("invalid cmd");
		return;
	}
	if(cmd->type >= CMD_END_CMDS)
	{
		_io->errorln("unsupported cmd type: ", cmd->type);
		return;
	}
	_executors[cmd->type](cmd);
}

void Csh::parse_and_execute(const string& str)
{
	try
	{
		if(str.empty()) return;
		auto cmd = parse_cmd(str);
		execute(cmd);
	}
	catch(cmd_parse_except& cpe)
	{
		_io->errorln("illegal cmd: ", cpe.what());
	}
	catch(exception& e)
	{
		_io->errorln("error: ", e.what());
	}
}

void Csh::run()
{
	while(_running)
	{
		_io->print(prompt_str(), " ");
		auto str = _io->getline();
		parse_and_execute(str);
	}
}

void Csh::execute_help(shared_ptr<cmd_t> raw)
{
	auto cmd = static_pointer_cast<cmd_help>(raw);
	_io->println(help_msg(cmd->type));
}
void Csh::execute_exit(shared_ptr<cmd_t> raw)
{
	close();
}
void Csh::execute_stat(shared_ptr<cmd_t> raw)
{
	_ua->status();
}
void Csh::execute_ping(shared_ptr<cmd_t> raw)
{
	_ua->ping();
}
void Csh::execute_connect(shared_ptr<cmd_t> raw)
{
	auto cmd = static_pointer_cast<cmd_connect>(raw);
	_ua->connect(cmd->ip, cmd->port);
}
void Csh::execute_reg(shared_ptr<cmd_t> raw)
{
	auto cmd = static_pointer_cast<cmd_reg>(raw);
	_ua->reg(cmd->name);
}
void Csh::execute_unreg(shared_ptr<cmd_t> raw)
{
	_ua->unreg();
}
void Csh::execute_room_list(shared_ptr<cmd_t> raw)
{
	_ua->room_list();
}
void Csh::execute_create_room(shared_ptr<cmd_t> raw)
{
	auto cmd = static_pointer_cast<cmd_create_room>(raw);
	_ua->create_room(make_shared<room_t>(cmd->name, cmd->psw));
}
void Csh::execute_join_room(shared_ptr<cmd_t> raw)
{
	auto cmd = static_pointer_cast<cmd_join_room>(raw);
	_ua->join_room(make_shared<room_t>(cmd->name, cmd->psw));
}
void Csh::execute_exit_room(shared_ptr<cmd_t> raw)
{
	_ua->exit_room();
}
void Csh::execute_ct(shared_ptr<cmd_t> raw)
{
	_ua->change_chess();
}
void Csh::execute_state(shared_ptr<cmd_t> raw)
{
	_ua->change_state();
}
void Csh::execute_match(shared_ptr<cmd_t> raw)
{
	_ua->start_match();
}
void Csh::execute_move(shared_ptr<cmd_t> raw)
{
	auto cmd = static_pointer_cast<cmd_move>(raw);
	_ua->move(cmd->x, cmd->y);
}
void Csh::execute_board(shared_ptr<cmd_t> raw)
{
	_ua->show_board();
}
