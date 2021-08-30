# gomoku-battle
RUA!

## Game

### Game Defination
- this game use a `15 * 15` board.
- two player in a game, use `black` or `white` chess.
- `white` chess always play first.

### Game Progress

#### v1
1. game svr running, two player ready.
2. player snd `[msg_reg]` to svr.
3. svr return `[msg_result]` to player.
4. if failed, change reg info and back to #2.
5. player snd `[msg_request]` to svr.
6. svr return `[msg_room_list]` to require room list.
7. player snd `[msg_room_oper]` to create or join a room.
8. svr return `[msg_result]` to player. if failed, back to #5.
9. player(we call A later) create the room will be its owner, and use black chess by default.
10. A can snd `[msg_chess]` to choose a chess type.
11. svr return `[msg_chess]` to all player to set chess type.
12. another player in room(we call B later) snd `[msg_state]` to set or cancel `ready` state.
13. svr snd `[msg_state]` to all player with B's state.
14. when B ready, A can start one match.
15. A snd `[msg_request]` to svr for starting game.
16. svr return `[msg_game]` to all player with a start sign.
17. from A start, players snd `[msg_move]` to svr, and svr snd `[msg_move]` to all player.
18. when game end after snd a `[msg_move]`, svr snd `[msg_game]` with an end info to each player.
19. after rcv `[msg_game]`, player snd `[msg_room_oper]` to `stay` or `exit` the room.
    if no one `stay`, room would be destroy. 
    if only one player `stay`, he will be new room owner.
    if all player `stay`, winner will become new room owner.
20. back to 9.

#### v2

- here was 5 states of player, `init` `idle`, `room`, `game`, `ERR`
  - when a player connect svr, this player will went into `init` state.
  - in any state, when a player lose connection to 'svr', or use too many illegal operations that reach svr's limits, player went into `ERR` state, and svr will end session with player.
- in `init` state
  player can only do registry.
  1. player was expected to snd `[msg_reg]` with a unique player name.
  2. svr rcv msg, and check msg type and player name. then return a `[msg_rslt]` with reg result.
  3. if reg succ, player went into `idle` state. if failed, back to #1.

- in `idle` state
  player can request [room list], or `create/join` a room. and maybe can request [room info] in future.
  - request [room list]
    1. player snd `[msg_request]` with `room list` code.
    2. svr return `[msg_roomlist]` to player.
  - `create` a room
    1. player snd `[msg_roomoper]` with `create` code and room info(name and maybe psw).
    2. svr try create room, and return `[msg_result]` with result code and msg.
    3. if succ create a room, player went into `room` state, and own the room created.
  - `join` a room
    1. player snd `[msg_roomoper]` with `join` code and room info(name and maybe psw).
    2. svr try put player into room, if succ, return `[msg_roominfo]` to all player in room. 
    3. return `[msg_roominfo]` with `join` sign to A, return `[msg_roominfo]` with `owner` sign to B.
    4. if failed, return `[msg_rslt]` to player with error code and msg.
    5. if succ join a room, player went `room` state.
  - `unreg`
- in `room` state
  we call a player A who own the room, and call another player B.
  player can `exit` room in this state;B can change ready state;when B ready, A can change his chess type(black or white), and `start` a game.
  and maybe a `kick out` in future.
  when a player `join/exit` the room, another player will rcv `[msg_roominfo]`.
  when A exit room, B become a new A.
  when all player exit, the room will be destroy.

  - `exit` room
    1. player snd `[msg_roomoper]` with `exit` code.
    2. svr return `[msg_result]`. if another player exists, svr snd `[msg_roominfo]`.
    3. player who exit room went into `idle` state.
  - 'ready/unready`
    1. B snd `[msg_state]` with state code(ready/unready).
    2. svr return `[msg_state]` to all player in room.
  - change chess
    1. A snd `[msg_chess]` with a chess type.
    2. svr return `[msg_chess]` to each player with this chess type.
    3. exp: A snd `[msg_chess(black)]`, svr return `[msg_chess(black)]` to A, `[msg_chess(white)]` to B.
  - `start` game
    1. A snd `[msg_request]` with game start sign.
    2. svr check status, if fail, return `[msg_rslt]` with error code and msg. if succ, return `[msg_game]` with start code. then all player went into `game` state.

- in `game` state
  `black` chess go first. players put chess one by one. until a player win, or no more space in board.
  - player movement
    1. a player snd `[msg_move]` with coord.
    2. svr check movements, if fail, return `[msg_rslt]` to this player.
    3. if succ, svr return `[msg_result]` with succ sign, return `[msg_move]` with chess type and coord to another player.
  - game end
    1. svr rcv a valid movement, and returned msg to player. 
    2. if a player won or no spaces remain in board, game snd `[msg_game]` with `win/lose/draw` sign to player. 
    3. all player went into `room` state.  

#### v3

first, devide server into 2 parts, I call `game svr` and `match svr`. as their name, a `game svr` handle `player` and `room` operations, like `login in` or `join/create room`. and, `match svr` handle gomoku matches.

all msg between svr and client contains `token` and `session` fields.

now svr only contains client registration, no user management. 

when a client register to `game svr` successfully, svr return a `token` to the clients.
if tcp connection closed or timeout without `unreg` operation, this associated player will be marked as `lost`.
clients can snd a new `reg` msg to svr with its token to reconnect to svr.
but, when a client `reg` to svr with a token, but this token associate to a 'connecting' user, or no user, svr will also generate a new token for it.


`session` was used to identify requests and its response. for exp, client snd a request to get exists roomlist, and svr snd a response back to client.  but before the roomlist response snd to client, there may be other same type msg snd to client. so, when client snd a request, mark it with a session id. svr will return response with same session id. then client can locate the msg needed.

##### token and session

`token`and `session` use `u64` data type, and use `-1` for invalid value.


##### game server

`game svr` handle player `registration`, `room operation`, and maybe some statistics in future.

`game svr` has almost nothing changed in interaction.

`game svr` listen to a fixed tcp port.

operations:
- register
  snd `msg_register`
  rcv `msg_result`
- unregister
  snd `msg_unregister` (not implemented)
  rcv `msg_result`
- room list
  snd `msg_request`
  rcv `msg_roomlist`
- create room
  snd `msg_oper`
  rcv `msg_result`
- join room
  snd `msg_oper`
  rcv `msg_result`
  rcv `msg_room_info` (owner)
- leave room
  snd `msg_oper`
  rcv `msg_result`
  rcv `msg_room_info` (remain player)
- change state
  snd `msg_state`
  rcv `msg_result`
  rcv `msg_room_info` (owner)
- change chesstype
  snd `msg_chess`
  rcv `msg_result`
  rcv `msg_room_info` (guest)
- start game
  snd `msg_game`
  rcv `msg_result`
  
so in `game svr`, svr rcv following msgs:
- `msg_register`
- `msg_unregister`
- `msg_request`
- `msg_oper`
- `msg_state`
- `msg_chess`
- `msg_game`
and snd following msgs:
- `msg_result`
- `msg_roomlist`
- `msg_room_info`

in some situation, some msg does not contains all infomation.
for example, a `msg_room_info` do not need room name or psw.

##### match server

a `match` comes from a room in `game svr`.

now, match svr doesn't use different process or connection.

to identify a match, use a fixed session id.
when room owner snd a `msg_game` to svr. svr generate a match from the room, and generate a session id(match id), and snd `msg_game` to players with this session id.

maybe i will really seperate match server and game server in future.
but, not now. when i need a distributed svr, maybe i will do that.

##### cui client

##### single thread

client
|
| connect
| register
|
idle <--------------|
|------------       |
|   |   |   |       |
|cr |jr |rl |ri     |
|   |   |   |       |
o<- g<- -------------
| | | |
|ct |st

...

may cause some problems.

##### msg callback


### Message Defination

all msg would be serialized in json format, and wrap with `<>`.
a msg packet would seems like `<{'key':'value'}>`.

#### msg_reg


## TODO

- [ ] catch `C-c` signal to shutdown svr.
- [ ] connection reuse for game svr.
- [ ] statistics of game.
- [ ] reconnect?
