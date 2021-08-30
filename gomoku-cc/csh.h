#pragma once
#include "cmds.h"
#include "user_agent.h"
#include <iostream>
using namespace std;

/*

*/
class Csh
{
private:
	ostream _os;
	char _prompt; // prompt char.
	shared_ptr<UserAgent> _ua;
	
	void validate_cmd(shared_ptr<cmd_t>);
	void execute(shared_ptr<cmd_t>);
public:
	Csh();
	~Csh();

	void run();
};
