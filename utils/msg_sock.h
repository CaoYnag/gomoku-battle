#pragma once
#include "msg.h"
#include "net_utils.h"

class MsgSock : public TcpSock
{
public:
    MsgSock();
    MsgSock(int port);
    MsgSock(const string& ip, int port);
    MsgSock(SOCK sock, const sockaddr_in& addr);
    virtual ~MsgSock();
public:
    template<class T>
    int send_msg(const T& msg)
    {
        auto str = serialize(msg);
        if(str.empty()) return 1;
        return send(str);
    }

    /* original msg snd/rcv */
    int rslt(u32 stat, const string& rslt);
    int req_rooms();
    int req(u32 target, u32 oper);
    int reg(const string& name);
    int roomlist(const vector<room_t>& rooms);
    int room_oper(u32 type, const room_t& room); // create/join/exit
    int create_room(const string& name, const string& psw);
    int join_room(const string& name, const string& psw);
    int exit_room(const string& name);
    int user_join_room(const string& name);
    int user_exit_room(const string& name);
    int roominfo(u32 type, const string& name);
    int choose_chess(u32 type);
    int set_chess(u32 type);
    int snd_chess(u32 type);
    int user_state(u32 state);
    int game_win();
    int game_lose();
    int game_draw();
    int snd_game(u32 state, u32 ex);
    int move(u32 x, u32 y);
    int move(u32 chess, u32 x, u32 y);
};