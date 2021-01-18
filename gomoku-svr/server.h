#include <boost/thread.hpp>
#include <map>
#include "../utils/msg_sock.h"
#include "user_agent.h"
using namespace std;

class Server
{
protected:
    /* Svr Configuration */
    int MAX_CONN;           // max connection svr accept.
    int MAX_CONN_PER_IP;    // max connection from same ip(avoid attack.)
    int OPER_TIMEOUT;      // timeout for each operation.
    int PORT;
protected:
    shared_ptr<MsgSock> _svr;
    int _conn;
    map<string, shared_ptr<UserAgent>> _conns;
    boost::mutex _guard;
    boost::atomic<int> _running;

    int init_params(const char* conf_path);
    int init_svr();
public:
    Server();
    virtual ~Server();

    int init(const char* conf_path);
    void service();
    void shutdown();
};