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
{}
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
        boost::thread(boost::bind(&GameSvr::handle_reg, this, cli)).detach();
    }
}

void GameSvr::handle_reg(shared_ptr<MsgSock> sock)
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
    auto reg_msg = static_pointer_cast<msg_reg>(msg);
    try
    {
        // generate token, add put it in conns
        auto agent = reg(sock, reg_msg->name, generate_token(sock->ip().c_str(), sock->port));
        _conns.put(agent->name, agent);
		msg_rslt rslt(RSLT_SUCSS, "success");
		rslt.token = agent->token();
        sock->send_msg(rslt);

        agent->mainloop();
    }
    catch(const std::exception& e)
    {
        // reg fail.
        sock->rslt(RSLT_FAIL, e.what());
    }
}
void GameSvr::release_conn(shared_ptr<MsgSock> sock)
{
    // TODO close conn, clear conns, failures and others
}

void GameSvr::shutdown()
{
    _running = false;
    // release all resource here.
}

shared_ptr<room_t> GameSvr::getroom(const string& name);
shared_ptr<UserAgent> GameSvr::reg(shared_ptr<MsgSock> sock, const string& name);
/* create a room, if succ, return created room pointer, if fail, throw an exception with error msg */
shared_ptr<room_t> GameSvr::create_room(const string& player, const string& name, const string& psw);
/*
* player join a room, if succ, return owner name.
* if fail, throw an exception.
* and if join succ, svr should notice room owner.
**/
string GameSvr::join_room(const string& player, const string& name, const string& psw);
/*
 * player exit room.
 * if fail, throw an exception.
 * if succ, notice remain player. if no player remain, destroy room.
 */
void GameSvr::exit_room(shared_ptr<room_t> room, const string& name);
/* get room list */
int GameSvr::roomlist(vector<room_t>& rooms);
/* change chess type */
int GameSvr::changechess(shared_ptr<room_t> room, const string& name, u32 ct);
/* change state */
void GameSvr::changestate(shared_ptr<room_t> room, const string& name, u32 state);
/*
 * start game
 * if succ, start a game thread.
 * if fail, throw an exception with failed msg.
 * failure situation:
 *   user not room owner
 *   player not enough or ready.
 */
void GameSvr::startgame(shared_ptr<room_t> room, const string& name);