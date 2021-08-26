#include "user_agent.h"
#include <unistd.h>
#include <thread>
using namespace std;

UserAgent::UserAgent()
{}

UserAgent::~UserAgent()
{}

shared_ptr<player_t> UserAgent::reg(const string& name)
{}
u32 UserAgent::unreg()
{}

u32 UserAgent::create_room(shared_ptr<room_t>)
{}

u32 UserAgent::join_room(shared_ptr<room_t>)
{}
u32 UserAgent::exit_room()
{}
u32 UserAgent::change_chess(u32 type)
{}
u32 UserAgent::change_state(u32 state)
{}
u32 UserAgent::start_match()
{}
u32 UserAgent::move(int x,  int y)
{}



void UserAgent::msg_proc()
{
	shared_ptr<msg_t> msg = nullptr;

	while(true)
	{
		msg = rcv_msg();
		{
			// TODO change to unique_lock and shared_lock.
			lock_guard<mutex> lock(_session_guard);
			if(_sessions.count(msg->session))
			{
				_sessions[msg->session]();
				_sessions.erase(msg->session);
			}
			else
				on_msg(msg);		
		}
	}
}

void UserAgent::on_msg(shared_ptr<msg_t> msg)
{}
