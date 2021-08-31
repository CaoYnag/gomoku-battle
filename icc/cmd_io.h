#pragma once
#include "cmds.h"
#include <stdexcept>
using namespace std;

int cmd_get_type(const string& cmd);
string unknwon_cmd_msg();
string bad_cmd_msg(int type);
string help_msg(int type = CMD_UNKNOWN);
shared_ptr<cmd_t> parse_cmd(const string& cmd);

class cmd_parse_except : public runtime_error
{
public:
	cmd_parse_except(const string& msg);
};
