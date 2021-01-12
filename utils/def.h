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
