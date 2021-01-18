#include "msg_sock.h"


MsgSock::MsgSock() : TcpSock()
{}
MsgSock::MsgSock(int port) : TcpSock(port)
{}
MsgSock::MsgSock(const string& ip, int port) : TcpSock(ip, port)
{}
MsgSock::MsgSock(SOCK sock, const sockaddr_in& addr) : TcpSock(sock, addr)
{}
MsgSock::~MsgSock()
{}

int MsgSock::rslt(u32 stat, const string& rslt)
{
    return send_msg(msg_result(stat, rslt));
}

int MsgSock::req_rooms()
{
    return req(0, 0); // TODO no other req now.
}

int MsgSock::req(u32 target, u32 oper)
{
    return send_msg(msg_request(target, oper));
}

int MsgSock::reg(const string& name)
{
    return send_msg(msg_reg(name));
}

int MsgSock::roomlist(const vector<room_t>& rooms)
{
    return send_msg(msg_roomlist(rooms));
}

int MsgSock::room_oper(u32 type, const room_t& room)
{
    return send_msg(msg_room_oper(type, room));
}

 // create/join/exit
int MsgSock::create_room(const string& name, const string& psw)
{
    return send_msg(msg_room_oper(ROOM_OPER_CREATE, room_t(0, name, psw, 0)));
}

int MsgSock::join_room(const string& name, const string& psw)
{
    return send_msg(msg_room_oper(ROOM_OPER_JOIN, room_t(0, name, psw, 0)));
}

int MsgSock::exit_room(const string& name)
{
    return send_msg(msg_room_oper(ROOM_OPER_EXIT, room_t(0, name, "", 0)));
}

int MsgSock::user_join_room(const string& name)
{
    return roominfo(ROOM_INFO_JOIN, name);
}

int MsgSock::user_exit_room(const string& name)
{
    return roominfo(ROOM_INFO_EXIT, name);
}

int MsgSock::roominfo(u32 type, const string& name)
{
    return send_msg(msg_room_info(type, name));
}

int MsgSock::choose_chess(u32 type)
{
    return send_msg(msg_chess(type));
}

int MsgSock::set_chess(u32 type)
{
    return send_msg(msg_chess(type));
}

int MsgSock::snd_chess(u32 type)
{
    return send_msg(msg_chess(type));
}

int MsgSock::user_state(u32 state)
{
    return send_msg(msg_state(state));
}

int MsgSock::game_win()
{
    return snd_game(GAME_STATE_END, GAME_RSLT_WIN);
}

int MsgSock::game_lose()
{
    return snd_game(GAME_STATE_END, GAME_RSLT_LOSE);
}

int MsgSock::game_draw()
{
    return snd_game(GAME_STATE_END, GAME_RSLT_DRAW);
}

int MsgSock::snd_game(u32 state, u32 ex)
{
    return send_msg(msg_game(state, ex));
}

int MsgSock::move(u32 x, u32 y)
{
    return move(0, x, y);
}

int MsgSock::move(u32 chess, u32 x, u32 y)
{
    return send_msg(msg_move(chess, x, y));
}
