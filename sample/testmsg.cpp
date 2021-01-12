#include "utils.h"
#include <iostream>
using namespace std;

void promsg(const char* msg)
{
    cout << msg << endl;
    cout << parseMSG(msg)->info() << endl;
}
int main()
{
    char* msg1 = "S:16.12.181.55:11111";
    char* msg2 = "H";
    char* msg3 = "R:16.12.181.59:22222";
    char* msg4 = "D:Hello,World!";
    promsg(msg1);
    promsg(msg2);
    promsg(msg3);
    promsg(msg4);
}