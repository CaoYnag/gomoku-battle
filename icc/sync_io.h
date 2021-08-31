#pragma once
#include <mutex>
#include <iostream>
#include <string>
using namespace std;

#define DEFUN(ret, foo_name) template<typename... Args> ret foo_name(Args&&... args)

class SyncIO
{
	mutex _i;
	mutex _o;
public:
	SyncIO();
	SyncIO(const SyncIO&) = delete;
	~SyncIO();

	DEFUN(void, print)
	{
		lock_guard<mutex> lock(_o);
		(cout << ... << args);
	}
	DEFUN(void, println)
	{
		print(forward<Args>(args)..., "\n");
	}
	DEFUN(void, error)
	{
		lock_guard<mutex> lock(_o);
		(cerr << ... << args);
	}
	DEFUN(void, errorln)
	{
		error(forward<Args>(args)..., "\n");
	}

	// input method
	string getline();
};
