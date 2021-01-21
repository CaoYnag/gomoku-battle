#include "user_agent.h"
#include "server.h"

constexpr const u32 USER_STATE_IDLE = 0;    // reged to svr, but did nothing
constexpr const u32 USER_STATE_ROOM = 1;    // created/joined a room
constexpr const u32 USER_STATE_GAME = 2;    // started a match
constexpr const u32 USER_STATE_ERR  = 0x8000;   // err/exited

UserAgent::UserAgent(shared_ptr<MsgSock> sock, u32 d, const string& name)
    : player_t(d, name), _sock(sock), _state(USER_STATE_IDLE)
{}

UserAgent::~UserAgent()
{}

int UserAgent::fail(u32 code, const string& msg)
{
    _sock->rslt(RSLT_FAIL, "invalid request.");
    if(Server::get()->fail(_sock))
    {
        // release resource here
        throw runtime_error("reach error limits.");
    }
}

void UserAgent::mainloop()
{
    shared_ptr<msg_t> msg = _sock->rcv_msg();

    while(_state != USER_STATE_ERR)
    {
        shared_ptr<msg_t> msg = _sock->rcv_msg();
        if(msg)
        {
            switch (_state)
            {
            case USER_STATE_IDLE: idle(msg);break;
            case USER_STATE_ROOM: room(msg);break;
            case USER_STATE_GAME: game(msg);break;
            
            default:
                break;
            }
        }
        else fail(RSLT_FAIL, "invalid msg.");
    }

}

int UserAgent::idle(shared_ptr<msg_t> msg)
{
    auto svr = Server::get();
    switch (msg->msg_type)
    {
    case MSG_T_REQUEST:
    {
        auto req = static_pointer_cast<msg_request>(msg);
        if(req->target == REQ_ROOM_LIST)
        {
            vector<room_t> rooms;
            svr->roomlist(rooms);
            _sock->roomlist(rooms);
        }
        else fail(RSLT_FAIL, "invalid request target.");
    }break;
    case MSG_T_ROOM_OPER:
    {
        auto oper = static_pointer_cast<msg_room_oper>(msg);
        if(oper->type == ROOM_OPER_CREATE)
        {
            try
            {
                auto room = svr->create_room(name, oper->room.name, oper->room.psw);
                _sock->rslt(RSLT_SUCSS, "success.");
                _state = USER_STATE_ROOM; // sucess create a room, enter `ROOM` state.
            } catch(exception& e)
            {
                fail(RSLT_FAIL, e.what());
            }
        }
        else if(oper->type == ROOM_OPER_JOIN)
        {
            try
            {
                auto owner = svr->join_room(name, oper->room.name, oper->room.psw);
                _sock->roominfo(ROOM_INFO_OWNER, owner);
                _state = USER_STATE_ROOM; // sucess create a room, enter `ROOM` state.
            } catch(exception& e)
            {
                fail(RSLT_FAIL, e.what());
            }
            
        }
        else fail(RSLT_FAIL, "invalid room operation.");
    }break;
    default:
        break;
    }
    return 0;
}
int UserAgent::room(shared_ptr<msg_t> msg)
{
    switch (msg->msg_type)
    {
    case MSG_T_CHESS:
    {
        // change chess type
    }break;
    case MSG_T_STATE:break;
    case MSG_T_ROOM_OPER:break;
    case MSG_T_REQUEST:break;
    default:
        break;
    }
    return 0;
}
int UserAgent::game(shared_ptr<msg_t> msg)
{}