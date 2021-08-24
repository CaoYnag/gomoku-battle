/*
 * Some global defination.
 */
#pragma once
#include <string>
using namespace std;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef u32 STATUS_CODE;


constexpr const int BOARD_SIZE = 15;
constexpr const int BOARD_POINT_NUM = BOARD_SIZE * BOARD_SIZE;
constexpr const int MAX_ILLEGAL_MOVE = 5;

constexpr const u32 INVALID_ID 			= -1;
constexpr const u64 INVALID_TOKEN 		= -1;
constexpr const u64 INVALID_SESSION 	= -1;

constexpr const u32 CHESS_BLANK       = 0x0;
constexpr const u32 CHESS_BLACK       = 0x1;
constexpr const u32 CHESS_WHITE       = 0x10;
constexpr const u32 CHESS_SUM         = CHESS_BLACK + CHESS_WHITE;

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
// 1. common
/* some return value */
constexpr const STATUS_CODE S_OK 					= 0x0; // success, no error
constexpr const STATUS_CODE S_NOCHANGE 				= 0x1; // no change.
constexpr const STATUS_CODE S_ERROR					= 0x2; // error, should describe detail in a text field
constexpr const STATUS_CODE S_ILLEGAL_OPER_LIMITS	= 0x3; // reach illegal oper limits
constexpr const STATUS_CODE S_INVALID_MSG			= 0x4; // invalid msg. unknown msg type, or cannot unpack
constexpr const STATUS_CODE S_ILLEGAL_MSG			= 0x5; // illegal msg. wrong msg parameters
constexpr const STATUS_CODE S_ILLEGAL_OPER			= 0x6; // illegal operations.

// 2. player
constexpr const STATUS_CODE S_PLAYER_INVALID_META 	= 0x101; // reg
constexpr const STATUS_CODE S_PLAYER_EXISTS 		= 0x102; // reg
constexpr const STATUS_CODE S_PLAYER_INVALID 		= 0x103; // any other opers, like join room
constexpr const STATUS_CODE S_PLAYER_BUSY			= 0x104; // unregister, join room
// maybe some token status here in future.

// 3. room
constexpr const STATUS_CODE S_ROOM_INVALID_META 	= 0x201; // create room
constexpr const STATUS_CODE S_ROOM_EXISTS 			= 0x202; // create room
constexpr const STATUS_CODE S_ROOM_NOT_EXISTS 		= 0x203; // join/exit room
constexpr const STATUS_CODE S_ROOM_FULL 			= 0x204; // join room
constexpr const STATUS_CODE S_ROOM_ILLEGAL_ACCESS	= 0x205; // join room
constexpr const STATUS_CODE S_ROOM_ALREADY_INSIDE	= 0x206; // join room
constexpr const STATUS_CODE S_ROOM_ILLEGAL_OPER 	= 0x207; // exit room, change ct/state, start game.
constexpr const STATUS_CODE S_ROOM_EMPTY			= 0x208; // exit room. need destroy room.

// 4. game
constexpr const STATUS_CODE S_GAME_NO_GUEST 		= 0x301; // start game
constexpr const STATUS_CODE S_GAME_ILLEGAL_OPER 	= 0x302; // guest start game
constexpr const STATUS_CODE S_GAME_NOT_PREPARE 		= 0x303; // guest not prepared

// 5. match
constexpr const STATUS_CODE S_MATCH_ILLEGAL_MOVE	= 0x401; // move
constexpr const STATUS_CODE S_MATCH_OPER_TIMEOUT	= 0x402; // player operation timeout.

string str_status_code(STATUS_CODE code);


enum MATCH_RESULTS
{
	MATCH_RESULT_DRAW = 0,
	MATCH_RESULT_OWNER_WIN,
	MATCH_RESULT_GUEST_WIN,

	/*  */
	MATCH_RESULT_ERROR,
};

struct match_result
{
	long begin;
	long end;
	int steps;
	u32 result;
};

struct player_records
{
	string player;
	long total; // total match
	long win; // win match
	long lose; // lose match
	f32 avg_step; // avg steps in all match.

	player_records();
	void game_win(int step);
	void game_lose(int step);
	void game_draw(int step);
private:
	void update_total_and_step(int step);
};

struct player_t
{
    u64 id;
    string name;
    u32 state;
    string ip;
    u32 port;

    player_t();
    player_t(const string& name);
    player_t(u64 id, const string& name, u32 state, const string& ip, u32 port);
};

struct room_t
{
    u64 id;
    string name;
    string psw;
    u32 oct;
	u32 gs; // guest state
    u32 state;
	string owner;
	string guest;

    room_t();
	room_t(const string& name);
    room_t(const string& name, const string& psw);
    room_t(u64 i, const string& n, const string& p);
    room_t(const room_t& room);
};
