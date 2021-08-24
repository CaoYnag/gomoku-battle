#pragma once
#include "../svr/match.h"
using namespace std;

class MatchRunner : public MatchExecutor
{
public:
	virtual void new_match(shared_ptr<Match> m);
};
