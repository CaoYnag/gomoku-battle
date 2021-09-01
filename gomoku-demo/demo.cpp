#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <conio.h>
using namespace std;

// ○●◎◇◆□■△▲
const char CHESS_BLACK_CHAR = 'X';
const char CHESS_WHITE_CHAR = 'O';
const char CHESS_BLANK_CHAR = '_';

const int CHESS_BLANK = 0;
const int CHESS_BLACK = 1;
const int CHESS_WHITE = 2;

constexpr const int BOARD_SIZE = 15;
constexpr const int BOARD_POINT_NUM = BOARD_SIZE * BOARD_SIZE;
class Gomoku
{
public:
    int* _board;
    int _size;
    int _num;
    int _state;

    Gomoku()
    {
        _size = BOARD_SIZE;
        _num = BOARD_POINT_NUM;
        _board = new int[_num];
    }
	Gomoku(const Gomoku&) = delete;
    ~Gomoku()
    {
        delete[] _board;
    }

    int get(int x, int y)
    {
        return _board[y * _size + x];
    }
    void set(int x, int y, int v)
    {
        _board[y * _size + x] = v;
    }

    void draw_board()
    {
        for(int i = 0; i < _num; ++i)
        {
            if(i % _size == 0) cout << "\n";
            switch (_board[i])
            {
            case CHESS_BLANK: cout << CHESS_BLANK_CHAR; break;
            case CHESS_BLACK: cout << CHESS_BLACK_CHAR; break;
            case CHESS_WHITE: cout << CHESS_WHITE_CHAR; break;
            default: cout << "?";
            }
        }
        cout << endl;
    }

    void start()
    {
        memset(_board, 0, sizeof(int) * _num);
        _state = 0;
    }
    int end()
    {
        return _state;
    }
    void check(int x, int y)
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
    int put(int x, int y, int v)
    {
        int e = get(x, y);
        if(e) return e;
        set(x, y, v);
        check(x, y);
        return 0;
    }
};


int main(int argc, char** argv)
{
    Gomoku gomoku;
    gomoku.start();
    int x, y;
    int type = 1;
    while(!gomoku.end())
    {
        // system("clear");
        gomoku.draw_board();
        int ret = 1;
        while(ret)
        {
            if(type == CHESS_BLACK)
                cout << "black:";
            else cout << "white:";
            cin >> x >> y;
            ret = gomoku.put(x, y, type);
        }
        type = 3 - type;
    }
    cout << (gomoku.end() == CHESS_BLACK ? "BLACK" : "WHITE") << " WON!" << endl;
    return 0;
}
