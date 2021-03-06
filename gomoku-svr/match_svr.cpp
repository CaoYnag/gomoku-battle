#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <spdlog/spdlog.h>
#include "server.h"
#include "def.h"


shared_ptr<Server> Server::_inst = nullptr;

shared_ptr<Server> Server::get()
{
    if(!_inst) _inst = shared_ptr<Server>(new Server);
    return _inst;
}

Server::Server() : _running(0)
{}
Server::~Server()
{
    _svr->close();
}

int Server::init()
{
    _svr = make_shared<MsgSock>(GlobalConf::get()->PORT);
    _svr->listen();
    spdlog::info("server listening in [{}].", GlobalConf::get()->PORT);
    return 0;
}

void Server::service()
{
    while(_running)
    {
        auto cli = _svr->accept_msg_sock();
        boost::thread(boost::bind(&Server::handle_reg, this, cli)).detach();
    }
}

void Server::handle_reg(shared_ptr<MsgSock> sock)
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
        auto agent = reg(sock, reg_msg->name);
        sock->rslt(RSLT_SUCSS, "success");

        agent->mainloop();
    }
    catch(const std::exception& e)
    {
        // reg fail.
        sock->rslt(RSLT_FAIL, e.what());
    }
}
void Server::release_conn(shared_ptr<MsgSock> sock)
{
    // TODO close conn, clear conns, failures and others
}

void Server::shutdown()
{
    _running = false;
    // release all resource here.
}
