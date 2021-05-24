#pragma once

struct GlobalConf
{
    int MAX_CONN;           // max connection svr accept.
    int MAX_CONN_PER_IP;    // max connection from same ip(avoid attack.)
    int OPER_TIMEOUT;       // timeout for each operation.
    int PORT;               // svr port
    int CTRL_PORT;          // ctrl api port
    int MAX_ILLEGAL_OPER;   // max illegal operaion of user. force close conn to user if reach this limit.

    static int init(const char* conf_path);
    static GlobalConf* _inst;
    static GlobalConf* get();
private:
    GlobalConf() = delete;
    GlobalConf(const GlobalConf&) = delete;
};