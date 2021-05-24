#include "def.h"
#include <spdlog/spdlog.h>
#include "../utils/config.h"

GlobalConf* GlobalConf::_inst = nullptr;
int GlobalConf::init(const char* conf_path)
{
    _inst = new GlobalConf();
    auto conf = spes::conf::get(conf_path);
    if(!conf)
    {
        spdlog::error("error read conf file [{}]", conf_path);
        return 1;
    }

    _inst->PORT = conf->GetInt("port");
    _inst->CTRL_PORT = conf->GetInt("ctrl-port");
    _inst->MAX_CONN = conf->GetInt("max-connections");
    _inst->MAX_CONN_PER_IP = conf->GetInt("max-connection-per-ip");
    _inst->OPER_TIMEOUT = conf->GetInt("operation-timeout");
    _inst->MAX_ILLEGAL_OPER = conf->GetInt("max-illegal-oper");

    spdlog::info("svr configurations:\n\tport: {}\n\tconn limit: {} {}\n\toper timeout: {}s.\n",
        _inst->PORT, _inst->MAX_CONN, _inst->MAX_CONN_PER_IP, _inst->OPER_TIMEOUT);
}
GlobalConf* GlobalConf::get()
{
    return _inst;
}