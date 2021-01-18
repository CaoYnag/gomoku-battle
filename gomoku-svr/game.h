#pragma once
#include "../utils/net_utils.h"
#include "def.h"
#include <vector>
#include <memory>
using namespace std;

class Game
{
protected:
    vector<shared_ptr<room_t>> _rooms;
    vector<shared_ptr<player_t>> _players;
    boost::mutex _guard;
public:
    Game();
    virtual ~Game();

    // handle new connection.
    void add_player(SOCK cli, sockaddr_in* from);
};

// a real match
class Match
{
protected:
    shared_ptr<Room> _room;
public:
    Match(shared_ptr<Room> room);
    virtual ~Match();
};