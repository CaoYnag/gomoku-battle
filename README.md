# gomoku-battle
RUA!

## Game

### Game Defination
- this game use a `15 * 15` board.
- two player in a game, use `black` or `white` chess.
- `black` chess always play first.

### Game Progress
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


### Message Defination

## TODO

- [ ] catch `C-c` signal to shutdown svr.