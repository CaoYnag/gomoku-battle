#include "msg.h"
using namespace std;

msg_result::msg_result() {}
msg_result::msg_result(u32 s, const string& r) : status(s), result(r)
{}

msg_request::msg_request() {}
msg_request::msg_request(u32 t, u32 o) : target(t), oper(o)
{}

msg_reg::msg_reg() {}
msg_reg::msg_reg(const string& n) : name(n)
{}

msg_roomlist::msg_roomlist() {}
msg_roomlist::msg_roomlist(const vector<room_t>& rms) : rooms(rms) {}

msg_room_oper::msg_room_oper() {}
msg_room_oper::msg_room_oper(u32 t, const room_t& r) : type(t), room(r) {}

msg_room_info::msg_room_info() {}
msg_room_info::msg_room_info(u32 t, const string& n) : type(t), name(n) {}

msg_chess::msg_chess() {}
msg_chess::msg_chess(u32 t) : type(t) {}

msg_state::msg_state() {}
msg_state::msg_state(u32 s) : state(s) {}

msg_game::msg_game() {}
msg_game::msg_game(u32 s, u32 e) : state(s), ex(e) {}

msg_move::msg_move() {}
msg_move::msg_move(u32 xx, u32 yy) : x(xx), y(yy) {}
msg_move::msg_move(u32 c, u32 xx, u32 yy) : chess(c), x(xx), y(yy) {}




u32 msg_type(const msg_raw_t& raw)
{
    if(raw.length() == 0) return MSG_T_UNKNOWN;
    msg_t m;
    try
    {
        deserialize(raw, m);
        return m.msg_type;
    }
    catch(const std::exception& e)
    {
        return MSG_T_UNKNOWN;
    }
    
}
msg_result unpack_result(const msg_raw_t& raw)
{
    msg_result ret;
    deserialize(raw, ret);
    return ret;
}
msg_request unpack_request(const msg_raw_t& raw)
{
    msg_request ret;
    deserialize(raw, ret);
    return ret;
}
msg_reg unpack_reg(const msg_raw_t& raw)
{
    msg_reg ret;
    deserialize(raw, ret);
    return ret;
}
msg_roomlist unpack_roomlist(const msg_raw_t& raw)
{
    msg_roomlist ret;
    deserialize(raw, ret);
    return ret;
}
msg_room_oper unpack_roomoper(const msg_raw_t& raw)
{
    msg_room_oper ret;
    deserialize(raw, ret);
    return ret;
}
msg_room_info unpack_roominfo(const msg_raw_t& raw)
{
    msg_room_info ret;
    deserialize(raw, ret);
    return ret;
}
msg_chess unpack_chess(const msg_raw_t& raw)
{
    msg_chess ret;
    deserialize(raw, ret);
    return ret;
}
msg_state unpack_state(const msg_raw_t& raw)
{
    msg_state ret;
    deserialize(raw, ret);
    return ret;
}
msg_game unpack_game(const msg_raw_t& raw)
{
    msg_game ret;
    deserialize(raw, ret);
    return ret;
}
msg_move unpack_move(const msg_raw_t& raw)
{
    msg_move ret;
    deserialize(raw, ret);
    return ret;
}