#pragma once
#include "def.h"

class Gomoku
{
public:
    int* _board;
    int _size;
    int _num;
    int _state;

    Gomoku();
	Gomoku(const Gomoku&) = delete;
    virtual ~Gomoku();
protected:
    void check(int x, int y);
    void set(int x, int y, int v);
public:
    void start();
    int end();

    int get(int x, int y);
    int put(int x, int y, int v);
};
