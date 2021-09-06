#include "cmd_io.h"
#include <sstream>
#include <vector>
#include <spes/utils.h>
using namespace std;

const string CMDS[] =
{
	"help",
	"exit",
	"stat",
	"ping",
	"conn",
	"reg",
	"unreg",
	"rl",
	"rc",
	"rj",
	"rq",
	"rt",
	"rs",
	"start",
	"mv",
	"board"
};

int cmd_get_type(const string& cmd)
{
	for(int i = 0; i < CMD_END_CMDS; ++i)
	{
		if(cmd == CMDS[i])
			return i;
	}
	return CMD_UNKNOWN;
}

typedef shared_ptr<cmd_t> (*cmd_parse_foo)(vector<string>&);

string unknown_cmd_msg()
{
	return "unknown cmd, type help show list all cmds.";
}

string bad_cmd_msg(int type)
{
	stringstream ss;
	ss << "bad cmd,  type "
	   << CMDS[type]
	   << " for detail.";
	return ss.str();
}

string help_msg(int type)
{
	stringstream ss;
	// TODO
	switch(type)
	{
	case CMD_UNKNOWN:
	default:
		ss << "client cmds:\n"
		   << "\thelp \tshow this msg\n"
		   << "\texit \texit client\n"
		   << "\tstat \tshow status\n"
		   << "\tping \tshow delay\n"
		   << "game cmds:\n"
		   << "\tconn \tconnect to game svr\n"
		   << "\treg  \tregister player\n"
		   << "\tunreg\tunregister player\n"
		   << "room cmds:\n"
		   << "\trc   \tcreate room\n"
		   << "\trj   \tjoin room\n"
		   << "\trq   \tquit room\n"
		   << "\trt	  \tchange chess\n"
		   << "\trs   \tchange state\n"
		   << "\tstart\tstart game\n"
		   << "game cmds:\n"
		   << "\tmv   \tmake a move\n"
		   << "\tboard\tshow board\n"
		   << "\ntype help [cmd] for detail\n";
	}
	return ss.str();;
}

shared_ptr<cmd_t> parse_help(vector<string>& tokens)
{
	int type = CMD_UNKNOWN;
	if(tokens.size() >= 2)
		type = cmd_get_type(tokens[1]);
	return make_shared<cmd_help>(type);
}
shared_ptr<cmd_t> parse_exit(vector<string>& tokens)
{
	return make_shared<cmd_exit>();
}
shared_ptr<cmd_t> parse_stat(vector<string>& tokens)
{
	return make_shared<cmd_stat>();
}
shared_ptr<cmd_t> parse_ping(vector<string>& tokens)
{
	return make_shared<cmd_ping>();
}
shared_ptr<cmd_t> parse_connect(vector<string>& tokens)
{
	if(tokens.size() < 3) throw cmd_parse_except("ip and port needed.");
	string ip = tokens[1];
	int port = my_atoi(tokens[2]);
	return make_shared<cmd_connect>(ip, port);
}
shared_ptr<cmd_t> parse_register(vector<string>& tokens)
{
	if(tokens.size() < 2) throw cmd_parse_except("need player name.");
	return make_shared<cmd_reg>(tokens[1]);
}
shared_ptr<cmd_t> parse_unreg(vector<string>& tokens)
{
	return make_shared<cmd_unreg>();
}
shared_ptr<cmd_t> parse_create_room(vector<string>& tokens)
{
	if(tokens.size() < 2) throw cmd_parse_except("room name is necessary.");
	string name = tokens[1];
	string psw;
	if(tokens.size() >= 3) psw = tokens[2];
	return make_shared<cmd_create_room>(name, psw);
}
shared_ptr<cmd_t> parse_join_room(vector<string>& tokens)
{
	if(tokens.size() < 2) throw cmd_parse_except("room name is necessary.");
	string name = tokens[1];
	string psw;
	if(tokens.size() >= 3) psw = tokens[2];
	return make_shared<cmd_join_room>(name, psw);
}
shared_ptr<cmd_t> parse_exit_room(vector<string>& tokens)
{	
	return make_shared<cmd_exit_room>();
}
shared_ptr<cmd_t> parse_room_ct(vector<string>& tokens)
{
	return make_shared<cmd_ct>();
}
shared_ptr<cmd_t> parse_room_state(vector<string>& tokens)
{
	return make_shared<cmd_state>();
}
shared_ptr<cmd_t> parse_start_match(vector<string>& tokens)
{
	return make_shared<cmd_match>();
}
shared_ptr<cmd_t> parse_move(vector<string>& tokens)
{
	if(tokens.size() < 3) throw cmd_parse_except("coord needed.");
	int x = my_atoi(tokens[1]);
	int y = my_atoi(tokens[2]);
	return make_shared<cmd_move>(x, y);
}
shared_ptr<cmd_t> parse_board(vector<string>& tokens)
{
	return make_shared<cmd_board>();
}
shared_ptr<cmd_t> parse_room_list(vector<string>& tokens)
{
	return make_shared<cmd_room_list>();
}

cmd_parse_foo CMD_PARSE_FOOS[] =
{
	parse_help,
	parse_exit,
	parse_stat,
	parse_ping,
	parse_connect,
	parse_register,
	parse_unreg,
	parse_room_list,
	parse_create_room,
	parse_join_room,
	parse_exit_room,
	parse_room_ct,
	parse_room_state,
	parse_start_match,
	parse_move,
	parse_board
};

shared_ptr<cmd_t> to_cmd(vector<string>& tokens)
{
	// clear tokens first.
	if(tokens.size())
	{
		int type = cmd_get_type(tokens[0]);
		if(type < CMD_END_CMDS) return CMD_PARSE_FOOS[type](tokens);
	}
	return make_shared<cmd_t>();
}

shared_ptr<cmd_t> parse_cmd(const string& cmd)
{
	vector<string> tokens;
	// tokenize
	string::size_type pos = 0;
	while(true)
	{
		auto np = cmd.find(" ", pos);
		if(np == string::npos)
		{
			tokens.emplace_back(cmd.substr(pos));
			break;
		}
		else tokens.emplace_back(cmd.substr(pos, np - pos));
		pos = np + 1;
	}
	return to_cmd(tokens);
}

cmd_parse_except::cmd_parse_except(const string& msg) : runtime_error(msg)
{}
