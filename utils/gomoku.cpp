#include "gomoku.h"
#include <cstring>

Gomoku::Gomoku()
{
    _size = BOARD_SIZE;
    _num = BOARD_POINT_NUM;
    _board = new int[_num];
}
Gomoku::~Gomoku()
{
    delete[] _board;
}

int Gomoku::get(int x, int y)
{
	if(x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return -1;
    return _board[y * _size + x];
}
void Gomoku::set(int x, int y, int v)
{
    _board[y * _size + x] = v;
}

void Gomoku::start()
{
    memset(_board, 0, sizeof(int) * _num);
    _state = 0;
}
int Gomoku::end()
{
    return _state;
}
void Gomoku::check(int x, int y)
{
    int v = get(x, y);
    int cnt;
    int xx, yy;
    
    // hor
    cnt = 1;
    xx = x;
    yy = y;
    while((--xx >= 0) && (get(xx, yy) == v)) ++cnt;
    xx = x;
    yy = y;
    while((++xx < _size) && (get(xx, yy) == v)) ++cnt;
    if(cnt > 4) 
    {
		_state = v;
		return;
    }
    
    
    // ver
    cnt = 1;
    xx = x;
    yy = y;
    while(--yy >= 0 && get(xx, yy) == v) ++cnt;
    xx = x;
    yy = y;
    while(++yy < _size && get(xx, yy) == v) ++cnt;
    if(cnt > 4) 
    {
		_state = v;
		return;
    }
    
    // nw
    cnt = 1;
    xx = x;
    yy = y;
    while(--yy >= 0 && --xx >= 0 && get(xx, yy) == v) ++cnt;
    xx = x;
    yy = y;
    while(++yy < _size && ++xx && get(xx, yy) == v) ++cnt;
    if(cnt > 4) 
    {
		_state = v;
		return;
    }
    
    // ne
    cnt = 1;
    xx = x;
    yy = y;
    while(++yy < _size && --xx >= 0 && get(xx, yy) == v) ++cnt;
    xx = x;
    yy = y;
    while(--yy >= 0 && ++xx && get(xx, yy) == v) ++cnt;
    if(cnt > 4) 
    {
		_state = v;
		return;
    }
}
int Gomoku::put(int x, int y, int v)
{
    int e = get(x, y);
    if(e) return e;
    set(x, y, v);
    check(x, y);
    return 0;
}
