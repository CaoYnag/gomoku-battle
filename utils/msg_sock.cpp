#include "msg_sock.h"
#include <spdlog/spdlog.h>

int MsgSock::MAX_ILLEGAL_OPER = INT_MAX;

MsgSock::MsgSock() : TcpSock(), _failures(0), _token(INVALID_TOKEN), _session(INVALID_SESSION)
{}
MsgSock::MsgSock(shared_ptr<TcpSock> ptr)
    : TcpSock(ptr->sock(), ptr->addr()), _failures(0), _token(INVALID_TOKEN), _session(INVALID_SESSION)
{}
MsgSock::MsgSock(int port) : TcpSock(port), _failures(0), _token(INVALID_TOKEN), _session(INVALID_SESSION)
{}
MsgSock::MsgSock(const string& ip, int port)
    : TcpSock(ip, port), _failures(0), _token(INVALID_TOKEN), _session(INVALID_SESSION)
{}
MsgSock::MsgSock(SOCK sock, const sockaddr_in& addr)
    : TcpSock(sock, addr), _failures(0), _token(INVALID_TOKEN), _session(INVALID_SESSION)
{}
MsgSock::~MsgSock()
{}

int MsgSock::fail()
{
    lock_guard<mutex> lock(_fguard);
    return ++_failures > MAX_ILLEGAL_OPER;
}

void MsgSock::max_failures(int v)
{
    MAX_ILLEGAL_OPER = v;
}

shared_ptr<MsgSock> MsgSock::accept_msg_sock()
{
    socklen_t len = sizeof(sockaddr_in);
    sockaddr_in addr;
    SOCK sock = ::accept(_sock, (sockaddr*)&addr, &len);
    if(sock > 0)
        return make_shared<MsgSock>(sock, addr);
    return nullptr;
}


shared_ptr<msg_t> MsgSock::rcv_msg()
{
    string msg_str;
    string rcv;
    auto pos = _tmp.find(PACKET_BEGIN_CHAR);
    while(pos == string::npos) // no valid data
    {
        rcv = recv();
        if(rcv.empty()) return nullptr; // no more data.
        _tmp = rcv;
        pos = _tmp.find(PACKET_BEGIN_CHAR);
        spdlog::debug("rcving data, buff remain: {}", _tmp);
    }
    if(pos > 0) // junk at packet head, cut off
    {
        _tmp = _tmp.substr(pos);
    }
    
    pos = _tmp.find(PACKET_END_CHAR);
    while(pos == string::npos) // not a complete msg
    {
        rcv = recv();
        if(rcv.empty()) return nullptr; // no more data.
        _tmp += rcv;
        pos = _tmp.find(PACKET_END_CHAR);
        spdlog::debug("rcving data, buff remain: {}", _tmp);
    }
    msg_str = _tmp.substr(1, pos - 1);
    _tmp = _tmp.substr(pos + 1);
    spdlog::debug("got msg {}, buff remain: {}", msg_str, _tmp);

    if(msg_str.empty()) return nullptr;
    auto t = msg_type(msg_str);
    shared_ptr<msg_t> ret = nullptr;
    switch (t)
    {
    case MSG_T_RESULT:
		ret = unpack_result(msg_str);
		break;
    case MSG_T_REQUEST:
		ret = unpack_request(msg_str);
		break;
    case MSG_T_REGISTER:
		ret = unpack_reg(msg_str);
		break;
    case MSG_T_ROOM_LIST:
		ret = unpack_roomlist(msg_str);
		break;
    case MSG_T_ROOM:
		ret = unpack_room(msg_str);
		break;
    case MSG_T_CHESS:
		ret = unpack_chess(msg_str);
		break;
    case MSG_T_STATE:
		ret = unpack_state(msg_str);
		break;
    case MSG_T_GAME:
		ret = unpack_game(msg_str);
		break;
    case MSG_T_MOVE:
		ret = unpack_move(msg_str);
		break;
    case MSG_T_UNKNOWN:
    default:
        break;
    }
    return ret;
}

int MsgSock::rslt(u32 stat, const string& rslt)
{
    return send_msg(msg_result(stat, rslt));
}

int MsgSock::req_rooms()
{
    return req(REQ_ROOM_LIST, 0); // TODO no other req now.
}

int MsgSock::start_game()
{
	return req(REQ_GAME_START, 0);
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
int MsgSock::roominfo(u32 type, const room_t& room)
{
    return send_msg(msg_room(type, room));
}

int MsgSock::create_room(const string& name, const string& psw)
{
	return roominfo(RO_ROOM_CREATE, room_t(name, psw));
}
int MsgSock::join_room(const string& name, const string& psw)
{
	return roominfo(RO_ROOM_JOIN, room_t(name, psw));
}
int MsgSock::exit_room(const string& name)
{
	return roominfo(RO_ROOM_EXIT, room_t(name));
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

int MsgSock::game_start()
{
    return snd_game(GAME_STATE_RUNNING, 0);
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

int MsgSock::game_err()
{
    return snd_game(GAME_STATE_END, GAME_RSLT_ERROR);
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
