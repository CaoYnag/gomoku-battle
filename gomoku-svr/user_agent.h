#pragma once
#include "../utils/msg_sock.h"

class UserAgent
{
protected:
    shared_ptr<MsgSock> _sock;
public:
    UserAgent(shared_ptr<MsgSock> sock);
    virtual ~UserAgent();
};