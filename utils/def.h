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



constexpr const u32 CHESS_BLACK       = 0x1;
constexpr const u32 CHESS_WHITE       = 0x2;

constexpr const u32 ROOM_OPER_UNKNOWN = 0x0;
constexpr const u32 ROOM_OPER_CREATE  = 0x1;
constexpr const u32 ROOM_OPER_JOIN    = 0x2;
constexpr const u32 ROOM_OPER_EXIT    = 0x3;

constexpr const u32 GAME_TARGET_ROOM  = 0x1;

constexpr const u32 ROOM_INFO_UNKNOWN = 0x0;
constexpr const u32 ROOM_INFO_JOIN    = 0x1;
constexpr const u32 ROOM_INFO_EXIT    = 0x2;

constexpr const u32 USER_STATE_PREPARE = 0x1;
constexpr const u32 USER_STATE_READY   = 0x2;

constexpr const u32 GAME_STATE_WAITING = 0x1;
constexpr const u32 GAME_STATE_RUNNING = 0x2;
constexpr const u32 GAME_STATE_END     = 0x3;

constexpr const u32 GAME_RSLT_WIN      = 0x1;
constexpr const u32 GAME_RSLT_LOSE     = 0x2;
constexpr const u32 GAME_RSLT_DRAW     = 0x3;

struct player_t
{
    u32 id;
    string name;
    string ip;
    u32 port;

    player_t();
    player_t(u32 d, const string& n, const string& i, u32 p);
};

struct room_t
{
    u32 id;
    string name;
    string psw;
    u32 state;

    room_t();
    room_t(u32 i, const string& n, const string& p, u32 s);
};
