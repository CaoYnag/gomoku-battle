#include <boost/thread.hpp>
#include <map>
#include <vector>
#include "../utils/msg_sock.h"
#include "user_agent.h"
using namespace std;

class Server
{
protected:
    vector<shared_ptr<UserAgent>> _players;
    map<string, shared_ptr<room_t>> _rooms;
protected:
    shared_ptr<MsgSock> _svr;
    int _conn;
    map<string, shared_ptr<UserAgent>> _conns;
    map<shared_ptr<MsgSock>, int> _failures;
    boost::mutex _failure_guard;
    boost::atomic<int> _running;

public:
    virtual ~Server();

    int init();
    void service();
    void shutdown();

    bool fail(shared_ptr<MsgSock> sock);
    void release_conn(shared_ptr<MsgSock> sock);

    /* handle registration */
    void handle_reg(shared_ptr<MsgSock> sock);
public:
    /* some management intf */

    /* 
    * register player, if succ, generate id, add to players, return a useragent, 
    * if fail, throw an exception. 
    * NEVER return an empty pointer.
    **/
    shared_ptr<UserAgent> reg(shared_ptr<MsgSock> sock, const string& name);
    /* create a room, if succ, return created room pointer, if fail, throw an exception with error msg */
    shared_ptr<room_t> create_room(const string& player, const string& name, const string& psw);
    /* 
    * player join a room, if succ, return owner name. 
    * if fail, throw an exception. 
    * and if join succ, svr should notice room owner.
    **/
    string join_room(const string& player, const string& name, const string& psw);
    /* get room list */
    int roomlist(vector<room_t>& rooms);

private:
    Server();
    Server(const Server&) = delete;
    static shared_ptr<Server> _inst;
public:
    static shared_ptr<Server> get();
};