#include "match_svr.h"
#include "game_svr.h"
#include <thread>
using namespace std;

void MatchRunner::new_match(shared_ptr<Match> m)
{
	thread th([&]()
		{
			auto r = m->run();
			GameSvr::get()->match_end(m, r);
		});
}
