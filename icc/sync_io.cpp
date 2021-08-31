#include "sync_io.h"

SyncIO::SyncIO()
{}

SyncIO::~SyncIO()
{}

string SyncIO::getline()
{
	lock_guard<mutex> lock(_i);
	char buff[2048];
	cin.getline(buff, 2048);
	return string(buff);
}
