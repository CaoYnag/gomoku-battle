#include "game.h"
#include "server.h"

constexpr const u32 USER_STATE_IDLE = 0;    // reged to svr, but did nothing
constexpr const u32 USER_STATE_ROOM = 1;    // created/joined a room
constexpr const u32 USER_STATE_GAME = 2;    // started a match
constexpr const u32 USER_STATE_ERR  = 0x8000;   // err/exited

UserAgent::UserAgent(shared_ptr<MsgSock> sock, u32 id, const string& name)
    : player_t(id, name, 0, sock->ip(), sock->port()), _sock(sock), _state(USER_STATE_IDLE)
{}

UserAgent::~UserAgent()
{}

int UserAgent::fail(u32 code, const string& msg)
{
    _sock->rslt(RSLT_FAIL, "invalid request.");
    if(_sock->fail())
    {
        // release resource here
        throw runtime_error("reach error limits.");
    }
    return 0;
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
		_room = room;
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
		_room = svr->getroom(oper->room.name);
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
    auto svr = Server::get();
    switch (msg->msg_type)
    {
    case MSG_T_CHESS:
    {
        // change chess type
	auto ct = static_pointer_cast<msg_chess>(msg);
	svr->changechess(_room, name, ct->type);
    }break;
    case MSG_T_STATE:
    {
	// change state
	auto sm = static_pointer_cast<msg_state>(msg);
	try
	{
	    svr->changestate(_room, name, sm->state);
	} catch(exception& e)
	{
	    fail(RSLT_FAIL, e.what());
	}
    }break;
    case MSG_T_ROOM_OPER:
    {
	// only exit here now.
	auto oper = static_pointer_cast<msg_room_oper>(msg);
	if(oper->type == ROOM_OPER_EXIT)
	{
	    try
	    {
		svr->exit_room(_room, name);
		// if succ exit room, back to idle state
		_state = USER_STATE_IDLE;
	    } catch(exception& e)
	    {
		fail(RSLT_FAIL, "operation failed.");
	    }
	}
    }break;
    case MSG_T_REQUEST:
    {
	// only `start` request now.
	auto req = static_pointer_cast<msg_request>(msg);
	if(req->oper == REQ_GAME_START)
	{
	    try
	    {
		svr->startgame(_room, name);
	    } catch(exception& e)
	    {
		fail(RSLT_FAIL, e.what());
	    }
	}
    }break;
    default:
        break;
    }
    return 0;
}
int UserAgent::game(shared_ptr<msg_t> msg)
{
    auto svr = Server::get();
    switch (msg->msg_type)
    {
    case MSG_T_MOVE:
    {
        // change chess type
	auto ct = static_pointer_cast<msg_chess>(msg);
	svr->changechess(_room, name, ct->type);
    }break;
    // drop other msg.
    }
    return 0;
}

void UserAgent::changechess(u32 type)
{
    _chesstype = type;
    _sock->set_chess(type);
}

void UserAgent::roominfo(u32 type, const string& name)
{
    _sock->roominfo(type, name);
}

void UserAgent::game_start()
{
    _sock->game_start();
}
void UserAgent::game_win()
{
    _sock->game_win();
}
void UserAgent::game_lose()
{
    _sock->game_lose();
}
void UserAgent::game_draw()
{
    _sock->game_draw();
}
void UserAgent::game_error(const string& msg)
{
    _sock->game_err(msg);
}
void UserAgent::prev_move(shared_ptr<msg_move> msg)
{
    _sock->send_msg(*msg);
}

shared_ptr<msg_t> UserAgent::next_oper()
{
    return _sock->rcv_msg();
}

/*====================Room====================*/
Room::Room(u32 i, const string& n, const string& p, u32 s,
	   shared_ptr<UserAgent> owner)
    : room_t(i, n, p, s), _owner(owner), _owner_chess(CHESS_WHITE)
{}

Room::~Room()
{
    // nothing to do.
}

string Room::join(shared_ptr<UserAgent> user, const string& psw)
{
    // TODO need a lock here
    if(user == _owner || user == _guest)
	return "already in room";
    if(_guest) return "room full";
    if(psw != this->psw) return "wrong psw";
    _guest = user;
    return "";
}

u32 Room::leave(shared_ptr<UserAgent> user)
{
    if(user == _guest)
    {
	_guest = nullptr;
	return 1;
    }
    if(user == _owner)
    {
	if(_guest)
	{
	    _owner = _guest;
	    _guest = nullptr;
	    // so now, the new owner use another chess type.
	    return 1;
	}
	else return 0;
    }
    // just return 1 to avoid room destroy
    return 1;
}