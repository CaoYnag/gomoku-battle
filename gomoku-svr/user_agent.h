#pragma once
#include "../utils/msg_sock.h"

class UserAgent : public player_t
{
protected:
    shared_ptr<MsgSock> _sock;
    u32 _state;
protected:
    int idle(shared_ptr<msg_t> msg);
    int room(shared_ptr<msg_t> msg);
    int game(shared_ptr<msg_t> msg);

    int fail(u32 code, const string& msg);
public:
    UserAgent(shared_ptr<MsgSock> sock, u32 d, const string& name);
    virtual ~UserAgent();

    /* main loop, util close conn */
    void mainloop();
};