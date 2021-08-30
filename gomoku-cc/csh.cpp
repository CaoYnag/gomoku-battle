#include "csh.h"
#include "cmd_io.h"

Csh::Csh() : _os(cout), _ua(_os)
{}

Csh::~Csh()
{}

void Csh::validate_cmd(shared_ptr<cmd_t> cmd)
{}

void Csh::execute(shared_ptr<cmd_t> cmd)
{}

void Csh::run()
{}
