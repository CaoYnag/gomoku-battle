#include "user_agent.h"
#include <unistd.h>
#include <thread>
using namespace std;

enum PlayerState
{
	PS_UNREG,
	PS_IDLE, // reged
	PS_ROOMING,
	PS_ROOM,
	PS_MATCH
};

UserAgent::UserAgent(ostream& os) : _os(os), _state(PS_UNREG)
{
	_session = 0;
}

UserAgent::~UserAgent()
{}

void UserAgent::reg(const string& name)
{
	if(_player)
	{
		_os << "already registerd, unreg first." << endl;
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::reg(name);
	lock_guard<mutex> slock(_session_guard);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
			{
				os << "player " << name << " register failed: "
				   << str_status_code(msg->status) << endl;
			}
			else
			{
				_state = PS_REG;
				_player = make_shared<player_t>(name);
			}
		};
}
void UserAgent::unreg()
{
	// TODO finish unreg msg in msgsock first.
}

void UserAgent::create_room(shared_ptr<room_t> r)
{
	if(_state != PS_IDLE)
	{
		_os << "cannot create room now." << endl;
		return;
	}
	if(!r)
	{
		_os << "invalid room info." << endl;
		return;
	}

	lock_guard<mutex> lock(_sock_guard);
	++_session;
	_state = PS_ROOMING;
	_room = r;
	MsgSock::create_room(r->name, r->psw);
	lock_guard<mutex> slock(_session_guard);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
			{
				_os << "create room " << _room->name << " with psw " << _room->psw
					<< " failed: " << str_status_code(msg->status) << endl;
				_state = PS_IDLE; // recover state to idle
			}
			else
			{
				_os << "success created room " << _room->name << endl;
				_state = PS_ROOM;
			}
		};
}

void UserAgent::join_room(shared_ptr<room_t> r)
{
	if(_state != PS_IDLE)
	{
		_os << "cannot create room now." << endl;
		return;
	}
	if(!r)
	{
		_os << "invalid room info." << endl;
		return;
	}

	lock_guard<mutex> lock(_sock_guard);
	++_session;
	_state = PS_ROOMING;
	_room = r;
	MsgSock::join_room(r->name, r->psw);
	lock_guard<mutex> slock(_session);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
			{
				_os << "join room " << _room->name << " with psw " << _room->psw
					<< "failed: " << str_status_code(msg->status) << endl;
				_state = PS_IDLE;
			}
			else
			{
				_os << "success joined room " << _room-name << endl;
				_state = PS_ROOM;
			}
		};
	
}
void UserAgent::exit_room()
{
	if(_state != PS_ROOM)
	{
		_os << "not in any room" << endl;
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	_state = PS_ROOMING;
	MsgSock::exit_room(_room->name);
	lock_guard<mutex> slock(_session);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_os << "error when exit room " << _room->name
					<< ": " << str_status_code(msg->status) << endl;
			_state = PS_IDLE;			
		};
}

void UserAgent::change_chess()
{
	if(_state != PS_ROOM)
	{
		_os << "create/join room first" << endl;
		return;
	}
	if(_player->name != _room->owner)
	{
		_os << "not room owner" << endl;
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::choose_chess(CHESS_SUM - _room->oct);
	lock_guard<mutex> slock(_session);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_os << "error change chess: " << str_status_code(msg->status) << endl;
			else
			{
				_room->oct = CHESS_SUM - _room->oct;
				_os << "sucess changed chess" << endl;
			}
		};
	
}
void UserAgent::change_state()
{
	if(_state != PS_ROOM)
	{
		_os << "create/join room first" << endl;
		return;
	}
	if(_player->name != _room->guest)
	{
		_os << "not room guest." << endl;
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	int state = (PLAYER_STATE_PREPARE + PLAYER_STATE_READY) - _room->gs;
	MsgSock::user_state(gs);
	lock_guard<mutex> slock(_session);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_os << "error change states: " << str_status_code(msg->status) << endl;
			else
			{
				_room->gs = state;
				_os << "sucess changed states" << endl;
			}
		};
}
void UserAgent::start_match()
{
	if(_state != PS_ROOM)
	{
		_os << "create/join room first" << endl;
		return;
	}
	if(_player->name != _room->owner)
	{
		_os << "not room owner." << endl;
		return;
	}
	if(_room->gs != PLAYER_STATE_READY)
	{
		_os << "guest not ready" << endl;
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	int state = (PLAYER_STATE_PREPARE + PLAYER_STATE_READY) - _room->gs;
	MsgSock::game_start();
	lock_guard<mutex> slock(_session);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_os << "error start match: " << str_status_code(msg->status) << endl;
		};	
}
void UserAgent::move(int x,  int y)
{
	if(_state != PS_MATCH)
	{
		_os << "not in a matach" << endl;
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::move(x, y);
	MsgSock::game_start();
	lock_guard<mutex> slock(_session);
	_sessions[_session] = [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_os << "move failed: " << str_status_code(msg->status) << endl;
		};	
}


void UserAgent::msg_proc()
{
 	shared_ptr<msg_t> msg = nullptr;

	while(true)
	{
		msg = rcv_msg();
		{
			// TODO change to unique_lock and shared_lock.
			if(msg->msg_type == MSG_T_RESULT)
			{
				msg_result rslt = static_pointer_cast<msg_result>(msg);
				lock_guard<mutex> lock(_session_guard);
				if(_sessions.count(msg->session))
				{
					_sessions[msg->session](rslt);
					_sessions.erase(msg->session);
				}
				_os << "unknown msg_results: " << rslt->status << endl;
			}
			else
				on_msg(msg);
		}
	}
}

void UserAgent::on_msg(shared_ptr<msg_t> msg)
{
	switch(msg->msg_type)
	{
	case MSG_T_MOVE:
	{
		auto m = static_pointer_cast<msg_move>(msg);
		on_move(m->x, m->y);
	} break;
	case MSG_T_ROOM_LIST:
	{
		auto rl = static_pointer_cast<msg_roomlist>(msg);
		on_room_list(rl->rooms);
	} break;
	case MSG_T_ROOM:
	{
		auto ri = static_pointer_cast<msg_room>(msg);
		on_roon_info(ri->type, ri->room);
	} break;
	case MSG_T_GAME;
	{
		// start or end
		auto g = static_pointer_cast<msg_game>(msg);
		if(g->state == GAME_STATE_RUNNING)
			on_match_start();
		else if(g->state == GAME_STATE_END)
			on_match_end(g->ex);
	}break;
	}
}

void UserAgent::on_move(int x, int y)
{
	
}

void UserAgent::on_match_start()
{
	_state = PS_MATCH;
	_os << "match started" << endl;
}

void UserAgent::on_match_end(u64 rslt)
{
	_state == PS_ROOM;
	_os << "game end"
}

void UserAgent::on_room_list(const vector<room_t>& rooms)
{
	for(const auto& r : rooms)
	{
		_os << r->name << " " << r->state << endl;
	}
}

void UserAgent::on_room_info(u32 type, const room_t& room)
{
	_room = make_shared<room_t>(room);
	_os << "room info updated: " << room->name << ": " << room->psw << " - " << room->state
		<< "\nowner: " << room->owner << " ct: " << room->oct
		<< "\nguest: " << room->guest << " st: " << room->gs
		<< endl;
}
