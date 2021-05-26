#define BOOST_TEST_MODULE test_utils
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include "../../utils/utils.h"
using std::cout;
using std::endl;
using std::vector;

bool test_repeat(vector<u64>& v)
{
    std::sort(v.begin(), v.end());
    for(int i = 0; i < v.size() - 1; ++i)
	if(v[i] == v[i + 1])
	{
	    printf("[ERROR] %llx collide.\n", v[i]);
	    return true;
	}
    return false;
}

BOOST_AUTO_TEST_CASE(test_size)
{
    BOOST_REQUIRE_EQUAL(8, sizeof(u64));
    BOOST_REQUIRE_EQUAL(4, sizeof(u32));
}

BOOST_AUTO_TEST_CASE(test_same_ip_token)
{
    vector<u64> tokens;
    for(int i = 0; i < 10000; ++i)
	tokens.emplace_back(generate_token("127.0.0.1", 1));
    if(test_repeat(tokens)) BOOST_REQUIRE(false);
}

BOOST_AUTO_TEST_CASE(test_session)
{
    vector<u64> sessions;
    for(int i = 0; i < 100000; ++i) sessions.emplace_back(generate_session());
    if(test_repeat(sessions)) BOOST_REQUIRE(false);
}
