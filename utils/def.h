/*
 * Some global defination.
 */
#pragma once
#include <string>
using namespace std;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

typedef u32 STATUS_CODE;

/* some return value */
constexpr const int S_OK = 0x0;
constexpr const int S_NOCHANGE = 0x1;


constexpr const int BOARD_SIZE = 15;
constexpr const int BOARD_POINT_NUM = BOARD_SIZE * BOARD_SIZE;

constexpr const u32 ID_INVALID = -1;

constexpr const u32 CHESS_BLANK       = 0x0;
constexpr const u32 CHESS_BLACK       = 0x1;
constexpr const u32 CHESS_WHITE       = 0x10;
constexpr const u32 CHESS_SUM         = CHESS_BLACK + CHESS_WHITE;

constexpr const u32 RSLT_SUCSS          = 0x0;
constexpr const u32 RSLT_FAIL           = 0x1;
constexpr const u32 RSLT_NEED_REGISTER  = 0x2;

constexpr const u32 REQ_ROOM_LIST       = 0x1;
constexpr const u32 REQ_GAME_START      = 0x2;

constexpr const u32 ROOM_OPER_UNKNOWN = 0x0;
constexpr const u32 ROOM_OPER_CREATE  = 0x1;
constexpr const u32 ROOM_OPER_JOIN    = 0x2;
constexpr const u32 ROOM_OPER_EXIT    = 0x3;

constexpr const u32 PLAYER_STATE_IDLE    = 0x0;
constexpr const u32 PLAYER_STATE_PREPARE = 0x1;
constexpr const u32 PLAYER_STATE_OWNER   = 0x2;
constexpr const u32 PLAYER_STATE_READY   = 0x3;

/* not full, player can join this room */
constexpr const u32 ROOM_STATE_OPEN   = 0x0; 
constexpr const u32 ROOM_STATE_FULL   = 0x1;
constexpr const u32 ROOM_STATE_MATCH  = 0x2;

constexpr const u32 GAME_TARGET_ROOM  = 0x1;

constexpr const u32 ROOM_INFO_UNKNOWN = 0x0;
constexpr const u32 ROOM_INFO_JOIN    = 0x1;
constexpr const u32 ROOM_INFO_EXIT    = 0x2;
constexpr const u32 ROOM_INFO_OWNER   = 0x3;

constexpr const u32 USER_STATE_PREPARE = 0x1;
constexpr const u32 USER_STATE_READY   = 0x2;

constexpr const u32 GAME_STATE_WAITING = 0x1;
constexpr const u32 GAME_STATE_RUNNING = 0x2;
constexpr const u32 GAME_STATE_END     = 0x3;

constexpr const u32 GAME_RSLT_WIN      = 0x1;
constexpr const u32 GAME_RSLT_LOSE     = 0x2;
constexpr const u32 GAME_RSLT_DRAW     = 0x3;
constexpr const u32 GAME_RSLT_ERROR    = 0x4;

/* =============== some result code =============== */
// 1. player
constexpr const STATUS_CODE S_PLAYER_INVALID_NAME 	= 0x1; // reg
constexpr const STATUS_CODE S_PLAYER_EXISTS 		= 0x2; // reg
constexpr const STATUS_CODE S_PLAYER_INVALID 		= 0x3; // any other opers, like join room
constexpr const STATUS_CODE S_PLAYER_BUSY			= 0x4; // unregister, join room
// maybe some token status here in future.

// 2. room
constexpr const STATUS_CODE S_ROOM_INVALID_NAME 	= 0x11; // create room
constexpr const STATUS_CODE S_ROOM_EXISTS 			= 0x12; // create room
constexpr const STATUS_CODE S_ROOM_NOT_EXISTS 		= 0x13; // join/exit room
constexpr const STATUS_CODE S_ROOM_FULL 			= 0x14; // join room
constexpr const STATUS_CODE S_ROOM_ILLEGAL_PSW		= 0x15; // join room
constexpr const STATUS_CODE S_ROOM_ALREADY_INSIDE	= 0x16; // join room
constexpr const STATUS_CODE S_ROOM_ILLEGAL_OPER 	= 0x17; // exit room, change ct/state
constexpr const STATUS_CODE S_ROOM_EMPTY			= 0x19; // exit room. need destroy room.

// 3. game
constexpr const STATUS_CODE S_GAME_NO_GUEST 		= 0x21; // start game
constexpr const STATUS_CODE S_GAME_ILLEGAL_OPER 	= 0x22; // guest start game
constexpr const STATUS_CODE S_GAME_NOT_PREPARE 		= 0x23; // guest not prepared

string str_status_code(STATUS_CODE code);

struct player_t
{
    u32 id;
    string name;
    u32 state;
    string ip;
    u32 port;

    player_t();
    player_t(const string& name);
    player_t(u32 id, const string& name, u32 state, const string& ip, u32 port);
};

struct room_t
{
    u32 id;
    string name;
    string psw;
    u32 oct;
    u32 state;
	string owner;
	string guest;

    room_t();
    room_t(const string& name, const string& psw);
    room_t(u32 i, const string& n, const string& p, u32 oct, u32 s);
    room_t(const room_t& room);
};
