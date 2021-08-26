#pragma once
#include "cmds.h"
#include "user_agent.h"

/*

*/
class Csh
{
private:
	char _prompt; // prompt char.
	
	void validate_cmd(shared_ptr<cmd_t>);
	void execute(shared_ptr<cmd_t>);
public:
	Csh();
	~Csh();

	void run();
};
