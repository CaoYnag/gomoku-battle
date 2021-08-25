#pragma once
#include <string>
#include <memory>
using namespace std;

enum CMD_TYPE
{
	CMD_UNKNOWN,


	CMD_CONNECT,
	CMD_REGISTER,
	CMD_CREATE_ROOM,
	CMD_JOIN_ROOM,
	CMD_EXIT_ROOM,
	CMD_ROOM_CHESSTYPE,
	CMD_ROOM_STATE,
	CMD_START_MATCH,
	CMD_MOVE,
	CMD_UNREGISTER,
	CMD_EXIT
};

struct cmd_t
{
	int type;

	cmd_t();
	cmd_t(int type);
};

struct cmd_connect : public cmd_t
{
	string ip;
	int port;

	cmd_conncet(const string& ip, int port);
};

struct cmd_reg : public msg_t
{
	string name;

	cmd_reg(const string& name);
};

struct cmd_create_room : public msg_t
{
	string name;
	string psw;

	cmd_create_room(const string& name);
	cmd_create_room(const string& name, const string& psw);
};

struct cmd_join_room : public msg_t
{
	string name;
	string psw;

	cmd_join_room(const string& name);
	cmd_join_room(const string& name, const string& psw);
};

struct cmd_exit_room : public msg_t
{
	string name;

	cmd_exit_room(const string& name);
};

struct cmd_ct : public msg_t
{
	int ct;

	cmd_ct(int ct);
};

struct cmd_state : public msg_t
{
	int state;

	cmd_state(int state);
};

struct cmd_match : public msg_t
{
	cmd_match();
};

struct cmd_move : public msg_t
{
	int x, y;

	cmd_move(int x, int y);
};

struct cmd_unreg : public msg_t
{
	cmd_unreg();
};

struct cmd_exit : public msg_t
{
	cmd_exit();
};

shared_ptr<cmd_t> parse_cmd(const string& cmd);
