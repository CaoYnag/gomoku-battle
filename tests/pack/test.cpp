#include <iostream>
#include <string>
using namespace std;

string buff;
string pack(const string& data)
{
	string ret;
	buff += data;
	cout << "\n--reading data:" << data << endl;
	cout << "--now buff:" << buff << endl;
	auto pos = buff.find("{");
	if(pos < 0) //junk, drop all
	{
		buff = "";
		return ret;
	}
	else if(pos > 0) // junk at begin, cut off
		buff = buff.substr(pos);
	
	pos = buff.find('}');
	if(pos > 0) // already contain a complete msg
	{
		ret = buff.substr(1, pos - 1);
		buff = buff.substr(pos + 1);
		cout << "--after cut." << endl;
		cout << "--now ret:" << ret << endl;
		cout << "--now buff:" << buff << endl;
	}
	else; // incomplete msg.

	cout << "--return:" << ret << endl;
	return ret;
}

int main()
{
	cout << pack("{aaaa}") << endl;
	cout << pack("{bbbb}{cc") << endl;
	cout << pack("cc}{dd") << endl;
	cout << pack("dd}{eeee}") << endl;
	cout << pack("{ffff}{gggg}") << endl;
	cout << pack("{hh") << endl;
	cout << pack("hh}") << endl;
	cout << pack("xxx{iiii}") << endl;
	cout << pack("") << endl;

	return 0;
}