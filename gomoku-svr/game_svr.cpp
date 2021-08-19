#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <spdlog/spdlog.h>
#include "def.h"
using namespace std;

shared_ptr<GameSvr> GameSvr::_inst = nullptr;

shared_ptr<GameSvr> GameSvr::get()
{
    if(!_inst) _inst = shared_ptr<GameSvr>(new GameSvr);
    return _inst;
}

GameSvr::GameSvr() : _running(0)
{
	_game = make_shared<Game>();
}
GameSvr::~GameSvr()
{
    _svr->close();
}

int GameSvr::init()
{
    _svr = make_shared<MsgSock>(GlobalConf::get()->PORT);
    _svr->listen();
    spdlog::info("server listening in [{}].", GlobalConf::get()->PORT);
    return 0;
}

void GameSvr::service()
{
    while(_running)
    {
        auto cli = _svr->accept_msg_sock();
        boost::thread(boost::bind(&GameSvr::handle_new_conn, this, cli)).detach();
    }
}

STATUS_CODE GameSvr::reg(shared_ptr<MsgSock> sock, shared_ptr<msg_reg> msg)
{
	// just accept as a new player.
	// TODO token check and reconnection should be implemented in future.

	auto agent = make_shared<UserAgent>(sock, msg->name);
	auto s = _game->register_player(static_pointer_cast<player_t>(agent));

	if(s == S_OK)
	{
		// success register
		// generate token, create agent and put into map
		agent->token(generate_token(sock-ip().c_str(), sock->port()));
		sock->token(agent->token());
		_tokens[agent->token()] = agent->name;
		_conns[agent->name] = agent;
	}
	
	return s;
}

void GameSvr::handle_new_conn(shared_ptr<MsgSock> sock)
{
    auto msg = sock->rcv_msg();
    while(!msg || msg->msg_type != MSG_T_REGISTER)
    {
        if(sock->fail()) break;
        sock->rslt(RSLT_NEED_REGISTER, "register first.");
        msg = sock->rcv_msg(); // TODO maybe need to set timeout here.
    }

    if(!msg || msg->msg_type != MSG_T_REGISTER)
    {
        release_conn(sock);
        return;
    }
    auto rmsg = static_pointer_cast<msg_reg>(msg);

	auto s = reg(sock, rmsg);
	msg_result rslt(s);
	rslt.session = msg.session;
	if(!s) rslt.token = get_agent(rmsg->name)->token();
	sock->send_msg(rslt);
	
	if(!s)
	{
		auto agent = _conns[rmsg->name];
		agent->mainloop();
	}
 }
void GameSvr::release_conn(shared_ptr<MsgSock> sock)
{
    // TODO close conn, clear conns, failures and others
	sock->close();
}

void GameSvr::shutdown()
{
    _running = false;
    // release all resource here.
}

STATUS_CODE GameSvr::register_player(shared_ptr<player_t> player)
{
	return _game->resiger_player(player);
}
STATUS_CODE GameSvr::unregister_player(shared_ptr<player_t> player)
{
	return _game->unregister_player(player);
}
STATUS_CODE GameSvr::create_room(const string& player, shared_ptr<room_t> room)
{
	return _game->create_room(player, room);
}
STATUS_CODE GameSvr::join_room(const string& player, shared_ptr<room_t> room)
{
	auto s = _game->join_room(player, room);
	if(s == S_OK)
	{
		// notice owner here
		room = _game->get_room(room->name);
		get_agent(room->owner)->sock()->room_oper(RI_PLAYER_JOIN, room);
	}
	return s;
}
STATUS_CODE GameSvr::exit_room(const string& player, const string& room)
{
	auto s = _game->exit_room(player, room);
	if(s == S_OK)
	{
		auto r = _game->get_room(room);
		if(r && r->owner != player)
			get_agent(r->owner)->sock()->room_oper(RI_PLAYER_EXIT, r);
	}
	return s;
}
void GameSvr::roomlist(vector<room_t>& rooms)
{
	_game->roomlist(rooms);
}
STATUS_CODE GameSvr::change_ct(const string& room, const string& player, u32 ct)
{
	auto s = _game->change_ct(room, player, ct);
	if(s == S_OK)
	{
		// NOTICE guest
		auto r = get_room(room);
		get_agent(r->guest)->sock()->room_oper(RI_PLAYER_CHESS, r);
	}
	return s;
}
STATUS_CODE GameSvr::change_state(const string& room, const string& player, u32 state)
{
	auto s = _game->change_state(room, player, state);
	if(s == S_OK)
	{
		auto r = get_room(room);
		get_agent(r->owner)->sock()->room_oper(RI_PLAYER_STATE, r);
	}
	return s;
}
STATUS_CODE GameSvr::start_match(const string& room, const string& player)
{
	auto s = _game->start_match(room, player);
	if(s == S_OK)
	{
		// TODO how to notice another player?
	}
	return s;
}

