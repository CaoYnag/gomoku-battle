#pragma once
#include <string>
#include <memory>
using namespace std;

enum CMD_TYPE
{
	/* cmd operation */
	CMD_HELP,
	CMD_EXIT,
	CMD_STAT,
	CMD_PING,

	/* game operation */
	CMD_CONNECT,
	CMD_REGISTER,
	CMD_UNREGISTER,

	CMD_ROOM_LIST,
	CMD_CREATE_ROOM,
	CMD_JOIN_ROOM,
	CMD_EXIT_ROOM,
	CMD_ROOM_CHESSTYPE,
	CMD_ROOM_STATE,
	CMD_START_MATCH,

	CMD_MOVE,
	CMD_BOARD, // draw board
	
	CMD_END_CMDS, // just for iterate, add cmds above this

	CMD_UNKNOWN = 0x100
};

struct cmd_t
{
	int type;

	cmd_t();
	cmd_t(int type);
};

#define PURE_CMD(T) struct T : public cmd_t\
					{\
	T();\
	};

struct cmd_connect : public cmd_t
{
	string ip;
	int port;

	cmd_connect(const string& ip, int port);
};

struct cmd_reg : public cmd_t
{
	string name;

	cmd_reg(const string& name);
};

struct cmd_create_room : public cmd_t
{
	string name;
	string psw;

	cmd_create_room(const string& name);
	cmd_create_room(const string& name, const string& psw);
};

struct cmd_join_room : public cmd_t
{
	string name;
	string psw;

	cmd_join_room(const string& name);
	cmd_join_room(const string& name, const string& psw);
};

// struct cmd_exit_room : public cmd_t
// {
// 	string name;

// 	cmd_exit_room(const string& name);
// };

// struct cmd_ct : public cmd_t
// {
// 	int ct;

// 	cmd_ct(int ct);
// };

// struct cmd_state : public cmd_t
// {
// 	int state;

// 	cmd_state(int state);
// };


struct cmd_move : public cmd_t
{
	int x, y;

	cmd_move(int x, int y);
};

PURE_CMD(cmd_room_list)
PURE_CMD(cmd_exit_room)
PURE_CMD(cmd_ct)
PURE_CMD(cmd_state)
PURE_CMD(cmd_unreg)
PURE_CMD(cmd_exit)
PURE_CMD(cmd_stat)
PURE_CMD(cmd_board)
PURE_CMD(cmd_ping)
PURE_CMD(cmd_match)

struct cmd_help : public cmd_t
{
	int type;

	cmd_help(int type = CMD_UNKNOWN);
};

// struct cmd_match : public cmd_t
// {
// 	cmd_match();
// };

// struct cmd_unreg : public cmd_t
// {
// 	cmd_unreg();
// };

// struct cmd_exit : public cmd_t
// {
// 	cmd_exit();
// };

// struct cmd_help : public cmd_t
// {
// 	cmd_help();
// };

// struct cmd_



