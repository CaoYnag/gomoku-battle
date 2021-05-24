#include <boost/thread.hpp>
#include <map>
#include <vector>
#include "../utils/msg_sock.h"
#include "game.h"
using namespace std;

/**
 * GameSvr only contains intfs communicated with clients.
 * All game logic implementioned in Game.
*/
class GameSvr
{
protected:
    shared_ptr<MsgSock> _svr;
    int _conn;
    map<string, shared_ptr<UserAgent>> _conns;
    boost::atomic<int> _running;
public:
    virtual ~GameSvr();

    int init();
    void service();
    void shutdown();

    void release_conn(shared_ptr<MsgSock> sock);

    /* handle registration */
    void handle_reg(shared_ptr<MsgSock> sock);

    /* some data intf */
    shared_ptr<room_t> getroom(const string& name);
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
    /*
     * player exit room.
     * if fail, throw an exception.
     * if succ, notice remain player. if no player remain, destroy room.
     */
    void exit_room(shared_ptr<room_t> room, const string& name);
    /* get room list */
    int roomlist(vector<room_t>& rooms);
    /* change chess type */
    int changechess(shared_ptr<room_t> room, const string& name, u32 ct);
    /* change state */
    void changestate(shared_ptr<room_t> room, const string& name, u32 state);
    /* 
     * start game
     * if succ, start a game thread.
     * if fail, throw an exception with failed msg.
     * failure situation:
     *   user not room owner
     *   player not enough or ready.
     */
    void startgame(shared_ptr<room_t> room, const string& name);
private:
    GameSvr();
    GameSvr(const GameSvr&) = delete;
    static shared_ptr<GameSvr> _inst;
public:
    static shared_ptr<GameSvr> Get();
};
