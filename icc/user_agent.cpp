#include <spdlog/spdlog.h>
#include "user_agent.h"
#include <unistd.h>
#include <thread>
#include "csh.h"
using namespace std;

enum PlayerState
{
	PS_UNCONN,
	PS_UNREG,
	PS_IDLE, // reged
	PS_ROOMING,
	PS_ROOM,
	PS_MATCH
};


UserAgent::UserAgent(shared_ptr<SyncIO> io) : MsgSock(), _io(io), _running(true),
											  _state(PS_UNCONN), _chesstype(CHESS_BLANK)
{
	_session = 0;
	if(init())
		_io->errorln("failed init socket.");
}

UserAgent::~UserAgent()
{}

void UserAgent::close()
{
	_running = false;
	MsgSock::close();
}

void UserAgent::connect(const string & ip, int port)
{
	auto ret = MsgSock::connect(ip, port);
	if(ret) 
		_io->errorln("error connected to ", ip,  ":", port);
	else
	{
		// connected
		_state = PS_UNREG;
		_session = 0; // started session
		thread(bind(&UserAgent::msg_proc, this)).detach();
	}
}

void UserAgent::status()
{
	// TODO
	//_io->errorln("not implemented yet.");
	_io->println("STATUS");
	if(_state == PS_UNCONN)
	{
		_io->println("offline");
		return;
	}
	_io->println("svr: ", ip(), ":", port());
	if(_state == PS_UNREG)
	{
		_io->println("not registered.");
		return;
	}
	_io->println("player: ", _player->name);
	if(_state == PS_IDLE)
	{
		_io->println("not in any room.");
		return;
	}
	if(_state == PS_ROOMING)
		_io->println("creating/joining room.");
	show_room_info(*_room);
	if(_state == PS_MATCH)
		_io->println("in matching.");
}

void UserAgent::ping()
{
	_io->errorln("not implemented yet.");
}

void UserAgent::show_board()
{
	_io->errorln("not implemented yet.");
}

void UserAgent::reg(const string& name)
{
	if(_state != PS_UNREG)
	{
		_io->println("cannot reg now, may u need connect to a svr, or unreg exists player first.");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::reg(name);
	
	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
			{
				_io->errorln("player ", name, " register failed: ", str_status_code(msg->status));
			}
			else
			{
				_io->println("Welcome, ", name);
				_state = PS_IDLE;
				_player = make_shared<player_t>(name);
			}
		});
}
void UserAgent::unreg()
{
	// TODO finish unreg msg in msgsock and complete this.
	_state = PS_IDLE;
}

void UserAgent::room_list()
{
	if(_state == PS_UNCONN || _state == PS_UNREG)
	{
		_io->errorln("not connected or registered.");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::req_rooms(); // not result response. do not need session callbacks
	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			// only return msg_error when failed.
			_io->errorln("error get room list: ", str_status_code(msg->status));
		});
}

void UserAgent::create_room(shared_ptr<room_t> r)
{
	if(_state != PS_IDLE)
	{
		_io->errorln("cannot create room now.");
		return;
	}
	if(!r)
	{
		_io->errorln("invalid room info.");
		return;
	}

	lock_guard<mutex> lock(_sock_guard);
	++_session;
	_state = PS_ROOMING;
	_room = r;
	MsgSock::create_room(r->name, r->psw);

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
			{
				_io->println("create room ", _room->name, " with psw ", _room->psw
						  , " failed: ", str_status_code(msg->status));
				_state = PS_IDLE; // recover state to idle
			}
			else
			{
				_io->println("success created room ", _room->name);
				_state = PS_ROOM;
			}
		});
}

void UserAgent::join_room(shared_ptr<room_t> r)
{
	if(_state != PS_IDLE)
	{
		_io->println("cannot create room now.");
		return;
	}
	if(!r)
	{
		_io->println("invalid room info.");
		return;
	}

	lock_guard<mutex> lock(_sock_guard);
	++_session;
	_state = PS_ROOMING;
	_room = r;
	MsgSock::join_room(r->name, r->psw);

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
			{
				_io->println("join room ", _room->name, " with psw ", _room->psw,
							 " failed: ", str_status_code(msg->status));
				_state = PS_IDLE;
			}
			else
			{
				_io->println("success joined room ", _room->name);
				_state = PS_ROOM;
			}
		});
	
}
void UserAgent::exit_room()
{
	if(_state != PS_ROOM)
	{
		_io->println("not in any room\n");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	_state = PS_ROOMING;
	MsgSock::exit_room(_room->name);

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_io->println("error when exit room ", _room->name
						  , ": ", str_status_code(msg->status));
			_state = PS_IDLE;			
		});
}

void UserAgent::change_chess()
{
	if(_state != PS_ROOM)
	{
		_io->println("create/join room first.");
		return;
	}
	if(_player->name != _room->owner)
	{
		_io->println("not room owner.");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::choose_chess(CHESS_SUM - _room->oct);

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_io->println("error change chess: ", str_status_code(msg->status));
			else
			{
				_room->oct = CHESS_SUM - _room->oct;
				_io->println("sucess changed chess.");
			}
		});
	
}
void UserAgent::change_state()
{
	if(_state != PS_ROOM)
	{
		_io->println("create/join room first.");
		return;
	}
	if(_player->name != _room->guest)
	{
		_io->println("not room guest.");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	int state = (PLAYER_STATE_PREPARE + PLAYER_STATE_READY) - _room->gs;
	MsgSock::user_state(state);

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_io->println("error change states: ", str_status_code(msg->status));
			else
			{
				_room->gs = state;
				_io->println("sucess changed states.");
			}
		});
}
void UserAgent::start_match()
{
	if(_state != PS_ROOM)
	{
		_io->println("create/join room first.");
		return;
	}
	if(_player->name != _room->owner)
	{
		_io->println("not room owner.");
		return;
	}
	if(_room->gs != PLAYER_STATE_READY)
	{
		_io->println("guest not ready.");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::game_start();

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_io->println("error start match: ", str_status_code(msg->status));
		});	
}
void UserAgent::move(int x,  int y)
{
	if(_state != PS_MATCH)
	{
		_io->println("not in a match.");
		return;
	}
	lock_guard<mutex> lock(_sock_guard);
	++_session;
	MsgSock::move(x, y);
	MsgSock::game_start();

	emit(_session, [&](shared_ptr<msg_result> msg)
		{
			if(msg->status)
				_io->println("move failed: ", str_status_code(msg->status));
		});	
}


void UserAgent::msg_proc()
{
 	shared_ptr<msg_t> msg = nullptr;

	while(_running)
	{
		// TODO connection may lost here.
		msg = rcv_msg();
		{
			if(!msg)
			{
				_io->errorln("rcv invalid msg.");
				continue;
			}
			if(msg->msg_type == MSG_T_RESULT)
				consume(static_pointer_cast<msg_result>(msg));
			else
				on_msg(msg);
			erase(msg);
		}
	}
}

void UserAgent::consume(shared_ptr<msg_result> rslt)
{
	lock_guard<mutex> lock(_session_guard);
	if(_sessions.count(rslt->session))
		_sessions[rslt->session](rslt);
	
	else _io->println("msg_results [", rslt->status, "] for unknown session [", rslt->session, "].");
}

void UserAgent::erase(shared_ptr<msg_t> msg)
{
	lock_guard<mutex> lock(_session_guard);
	if(_sessions.count(msg->session))
	   _sessions.erase(msg->session);
}

void UserAgent::emit(u64 session, msg_callback cb)
{
	spdlog::debug("added session cb: {}", session);
	lock_guard<mutex> slock(_session_guard);
	_sessions[session] = cb;
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
		on_room_info(ri->type, ri->room);
	} break;
	case MSG_T_GAME:
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
	// TODO
}

void UserAgent::on_match_start()
{
	_state = PS_MATCH;
	_io->println("match started.");
}
string str_game_rslt(u64 code)
{
	switch(code)
	{
	case GAME_RSLT_WIN: return "win";
	case GAME_RSLT_LOSE: return "lose";
	case GAME_RSLT_DRAW: return "draw";
	case GAME_RSLT_ERROR: return "error";
	default: return "unknwon";
	}
}
void UserAgent::on_match_end(u64 rslt)
{
	_state = PS_ROOM;
	_io->println("game end: ", str_game_rslt(rslt));
}

void UserAgent::on_room_list(const vector<room_t>& rooms)
{
	for(const auto& r : rooms)
	{
		_io->println(r.name, " ", r.state);
	}
}

void UserAgent::on_room_info(u32 type, const room_t& room)
{
	_room = make_shared<room_t>(room);
	_io->println("room info updated.");
	show_room_info(room);
}

void UserAgent::show_room_info(const room_t& room)
{
	_io->println("room: ", room.name, ": ", room.psw, " - ", room.state,
				 "\nowner: ", room.owner, " ct: ", room.oct,
				 "\nguest: ", room.guest, " st: ", room.gs);
}
