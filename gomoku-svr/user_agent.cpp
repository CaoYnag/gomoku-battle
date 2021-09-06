#include "game_svr.h"
#include <unistd.h>

// these state only use for UserAgent
constexpr const u32 USER_STATE_IDLE = 0;    // reged to svr, but did nothing
constexpr const u32 USER_STATE_ROOM = 1;    // created/joined a room
constexpr const u32 USER_STATE_GAME = 2;    // started a match
constexpr const u32 USER_STATE_ERR  = 0x8000;   // err/exited

UserAgent::UserAgent(shared_ptr<MsgSock> sock, const string& name)
    : Player(INVALID_ID, name, sock->ip(), sock->port()), _sock(sock), _state(USER_STATE_IDLE), _token(INVALID_TOKEN)
{}

UserAgent::~UserAgent()
{}

int UserAgent::rslt(u32 code, const string& msg)
{
    _sock->rslt(code, msg);
    if(code == S_ILLEGAL_OPER && _sock->fail())
    {
        // release resource here
        throw runtime_error("reach error limits.");
    }
    return 0;
}
	
void UserAgent::mainloop()
{
	try
	{
		while(_state != USER_STATE_ERR)
		{
			shared_ptr<msg_t> msg = _sock->rcv_msg();
			if(msg)
			{
				_sock->session(msg->session); // change session here.
				switch (_state)
				{
				case USER_STATE_IDLE: idle(msg);break;
				case USER_STATE_ROOM: room(msg);break;
				case USER_STATE_GAME: game(msg);break;
					
				default:
					break;
				}
			}
			else
			{
				_sock->session(); // invalidate session.
				rslt(S_INVALID_MSG);
			}
		}
    } catch(exception& e)
	{
		// reach limit
		// keep last session.
		_sock->rslt(S_ILLEGAL_OPER_LIMITS); // do not use rslt().
	}
}

int UserAgent::idle(shared_ptr<msg_t> msg)
{
	auto svr = GameSvr::get();
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
        else rslt(S_ILLEGAL_MSG);
    }break;
    case MSG_T_ROOM:
    {
        auto oper = static_pointer_cast<msg_room>(msg);
        if(oper->type == RO_ROOM_CREATE)
        {
			auto s = svr->create_room(name, make_shared<room_t>(oper->room));
			if(!s)
			{
				_state = USER_STATE_ROOM; // sucess create a room, enter `ROOM` state.
				_room = svr->game()->get_room(oper->room.name)->_room;
			}
			rslt(s);
        }
        else if(oper->type == RO_ROOM_JOIN)
        {
			auto s = svr->join_room(name, make_shared<room_t>(oper->room));
			rslt(s);
			if(!s)
			{
				_state = USER_STATE_ROOM; // sucess create a room, enter `ROOM` state.
				_room = svr->game()->get_room(oper->room.name)->_room;
				_sock->roominfo(RI_PLAYER_JOIN, *_room);
				// TODO noticed owner in game_svr
			}
        }
        else rslt(S_ILLEGAL_OPER);
    }break;
    default:
		
        break;
    }
    return 0;
}

int UserAgent::room(shared_ptr<msg_t> msg)
{
	auto svr = GameSvr::get();
	
    switch (msg->msg_type)
    {
    case MSG_T_CHESS:
    {
        // change chess type
		auto ct = static_pointer_cast<msg_chess>(msg);
		auto s = svr->change_ct(_room->name, name, ct->type);
		rslt(s);
		// TODO notice guest here if success // done in GameSvr
    }break;
    case MSG_T_STATE:
    {
		// change state
		auto sm = static_pointer_cast<msg_state>(msg);
		auto s = svr->change_state(_room->name, name, sm->state);
		rslt(s);
		// TODO notice owner here // done in GameSvr
    }break;
    case MSG_T_ROOM:
    {
		// only exit here now.
		auto oper = static_pointer_cast<msg_room>(msg);
		if(oper->type == RO_ROOM_EXIT)
		{
			auto s = svr->exit_room(name, _room->name);

			if(!s)
			{
				// if succ exit room, back to idle state
				_state = USER_STATE_IDLE;
			}
			rslt(s);
		}
		else rslt(S_ILLEGAL_OPER); // only exit is legal
    }break;
    case MSG_T_REQUEST:
    {
		// only `start` request now.
		auto req = static_pointer_cast<msg_request>(msg);
		if(req->target == REQ_GAME_START)
		{
			auto s = svr->start_match(_room->name, name);
			if(!s)
			{
				// success, enter game state
				_state = USER_STATE_GAME;
			}
			rslt(s);
		} else rslt(S_ILLEGAL_OPER);
    }break;
    default:
        break;
    }
    return 0;
}
int UserAgent::game(shared_ptr<msg_t> msg)
{
    auto svr = GameSvr::get();
    switch (msg->msg_type)
    {
    case MSG_T_MOVE:
    {
		lock_guard<mutex> lock(_mg);
		_moves.emplace_back(static_pointer_cast<msg_move>(msg));
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
void UserAgent::game_error()
{
    _sock->game_err();
}
void UserAgent::prev_move(shared_ptr<msg_move> msg)
{
    _sock->send_msg(*msg);
}

shared_ptr<msg_t> UserAgent::next_oper()
{
    return _sock->rcv_msg();
}


/* ==================== Player intfs ==================== */
int UserAgent::next_move(u64 mid, int& x, int& y)
{
	while(true)
	{
		lock_guard<mutex> lock(_mg);
		if(!_moves.empty()) break;
		usleep(100000); // sleep 100ms and wait next msg.
	}
	lock_guard<mutex> lock(_mg);
	x = _moves[0]->x;
	y = _moves[0]->y;
	_moves.erase(_moves.begin());
	return S_OK;
}

void UserAgent::enemy_move(int x, int y)
{
	sock()->move(x, y);
}

void UserAgent::game_result(shared_ptr<match_result> rslt)
{
	bool is_owner = _room->name == name;
	switch(rslt->result)
	{
	case MATCH_RESULT_DRAW:
	{
		sock()->game_draw();
	} break;
	case MATCH_RESULT_OWNER_WIN:
	{
		if(is_owner) sock()->game_win();
		else sock()->game_lose();
	} break;
	case MATCH_RESULT_GUEST_WIN:
	{
		if(is_owner) sock()->game_lose();
		else sock()->game_win();
	} break;
	case MATCH_RESULT_ERROR:
	{
		sock()->game_err();
	} break;
	}
}

void UserAgent::notice(u32 stat)
{
	sock()->rslt(stat);
}
