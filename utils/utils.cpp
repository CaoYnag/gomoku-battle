#include "utils.h"
#include <climits>
#include "msg.h"
#include <spes/utils.h>

typedef union
{
    struct
    {
	u8 s1, s2, s3, s4;
    };
    u32 v;
} ipv4_t;

u64 generate_token(const char* ips, u16 port)
{
    u64 ret = 0;
    ipv4_t ip;
    sscanf(ips, "%d.%d.%d.%d", &ip.s1, &ip.s2, &ip.s3, &ip.s4);

    ret = ip.v;
    ret <<= 32;
    // ret |= port << 16;
    // NOTICE: ignore port.
    ret |= ((rand() % 0xffff) & 0xffff) << 16;
    ret |= (rand() % 0xffff) & 0xffff;
    return ret;
}

u64 generate_session()
{
    constexpr const u64 HALF = 0xffffffff;
    u64 ret = smush() % HALF;
    ret <<= 32;
    ret |= smush() % HALF;
    return ret;
}

