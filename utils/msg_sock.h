#pragma once
#include "msg.h"
#include "net_utils.h"

class MsgSock : public TcpSock
{
protected:
    string _tmp;
    const char PACKET_BEGIN_CHAR = '<';
    const char PACKET_END_CHAR = '>';
public:
    MsgSock();
    /* bugs when  */
    MsgSock(shared_ptr<TcpSock> ptr);
    MsgSock(int port);
    MsgSock(const string& ip, int port);
    MsgSock(SOCK sock, const sockaddr_in& addr);
    virtual ~MsgSock();

    shared_ptr<MsgSock> accept_msg_sock();
public:
    template<class T>
    int send_msg(const T& msg)
    {
        auto raw = pack(msg);
        if(raw.empty()) return 1;
        raw = PACKET_BEGIN_CHAR + raw + PACKET_END_CHAR;
        return send(raw);
    }
    shared_ptr<msg_t> rcv_msg();

    /* msg snd intfs */
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
    int game_start();
    int game_win();
    int game_lose();
    int game_draw();
    int game_err(const string& msg);
    int snd_game(u32 state, u32 ex);
    int move(u32 x, u32 y);
    int move(u32 chess, u32 x, u32 y);
};
