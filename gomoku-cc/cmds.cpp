#include "cmds.h"

cmd_t::cmd_t() {}
cmd_t::cmd_t(int t) : type(t) {}

cmd_connect::cmd_connect(const string& _ip, int _port) : cmd_t(CMD_CONNECT), ip(_ip), port(_port)
{}

cmd_create_room::cmd_create_room(const string& n) : cmd_t(CMD_CREATE_ROOM), name(n), psw("")
{}
cmd_create_room::cmd_create_room(const string& n, const string& p) : cmd_t(CMD_CREATE_ROOM), name(n), psw(p)
{}


cmd_join_room::cmd_join_room(const string& n) : cmd_t(CMD_JOIN_ROOM), name(n), psw("")
{}
cmd_join_room::cmd_join_room(const string& n, const string& p) : cmd_t(CMD_JOIN_ROOM), name(n), psw(p)
{}

cmd_exit_room::cmd_exit_room() : cmd_t(CMD_EXIT_ROOM)
{}

cmd_ct::cmd_ct() : cmd_t(CMD_ROOM_CHESSTYPE)
{}

cmd_state::cmd_state() : cmd_t(CMD_ROOM_STATE)
{}

cmd_match::cmd_match() : cmd_t(CMD_START_MATCH)
{}

cmd_move::cmd_move(int xx, int yy) : cmd_t(CMD_MOVE), x(xx), y(yy)
{}

cmd_unreg::cmd_unreg() : cmd_t(CMD_UNREGISTER)
{}

cmd_exit::cmd_exit() : cmd_t(CMD_EXIT)
{}

cmd_help::cmd_help(int t) : cmd_t(CMD_HELP), type(t)
{}

cmd_stat::cmd_stat() : cmd_t(CMD_STAT)
{}

cmd_board::cmd_board() : cmd_t(CMD_BOARD)
{}

cmd_ping::cmd_ping() : cmd_t(CMD_PING)
{}

cmd_reg::cmd_reg(const string& n) : cmd_t(CMD_REGISTER), name(n)
{}

cmd_room_list::cmd_room_list() : cmd_t(CMD_ROOM_LIST)
{}
