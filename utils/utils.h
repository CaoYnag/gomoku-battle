#pragma once
#include "def.h"

template<typename T>
class Wrapper
{
public:
    virtual ~Wrapper(){}

    virtual T wrap(T t)
    {
	return t;
    }
};

u64 generate_token(const char* ip, u16 port);
u64 generate_session();
