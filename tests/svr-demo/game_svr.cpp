#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <spdlog/spdlog.h>
#include "conf.h"
#include "../../utils/net_utils.h"
using namespace std;

shared_ptr<GameSvr> GameSvr::_inst = nullptr;

shared_ptr<GameSvr> GameSvr::get()
{
    if(!_inst) _inst = shared_ptr<GameSvr>(new GameSvr);
    return _inst;
}

GameSvr::GameSvr() : _running(0)
{
    _game = Game::get();
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
        msg = sock->rcv_msg();
    }

    if(!msg || msg->msg_type != MSG_T_REGISTER)
    {
        release_conn(sock);
        return;
    }
    auto reg_msg = static_pointer_cast<msg_reg>(msg);
    try
    {
        auto agent = reg(sock, reg_msg->name); // TODO add token here
	auto token = generate_token(agent->ip, agent->port);
        sock->rslt(RSLT_SUCSS, "success");

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
    sock->close();
}

void GameSvr::shutdown()
{
    _running = false;
    // release all resource here.
}
