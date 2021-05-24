#include "msg.h"
using namespace std;

msg_t::msg_t() : msg_type(MSG_T_UNKNOWN), token(TOKEN_INVALID), session(SESSION_INVALID) {}
msg_t::msg_t(u32 type) : msg_type(type), token(TOKEN_INVALID), session(SESSION_INVALID) {}
msg_t::msg_t(u32 type, u64 token, u64 session) : msg_type(type), token(token), session(session) {}

msg_result::msg_result() : msg_t(MSG_T_RESULT) {}
msg_result::msg_result(u32 s, const string& r) : msg_t(MSG_T_RESULT), status(s), result(r)
{}

msg_request::msg_request() : msg_t(MSG_T_REQUEST) {}
msg_request::msg_request(u32 t, u32 o) : msg_t(MSG_T_REQUEST), target(t), oper(o)
{}

msg_reg::msg_reg() : msg_t(MSG_T_REGISTER) {}
msg_reg::msg_reg(const string& n) : msg_t(MSG_T_REGISTER), name(n)
{}

msg_roomlist::msg_roomlist() : msg_t(MSG_T_ROOM_LIST) {}
msg_roomlist::msg_roomlist(const vector<room_t>& rms) : msg_t(MSG_T_ROOM_LIST), rooms(rms) {}

msg_room_oper::msg_room_oper() : msg_t(MSG_T_ROOM_OPER) {}
msg_room_oper::msg_room_oper(u32 t, const room_t& r) : msg_t(MSG_T_ROOM_OPER), type(t), room(r) {}

msg_room_info::msg_room_info() : msg_t(MSG_T_ROOM_INFO) {}
msg_room_info::msg_room_info(u32 t, const string& n) : msg_t(MSG_T_ROOM_INFO), type(t), name(n) {}

msg_chess::msg_chess() : msg_t(MSG_T_CHESS) {}
msg_chess::msg_chess(u32 t) : msg_t(MSG_T_CHESS), type(t) {}

msg_state::msg_state() : msg_t(MSG_T_STATE) {}
msg_state::msg_state(u32 s) : msg_t(MSG_T_STATE), state(s) {}

msg_game::msg_game() : msg_t(MSG_T_GAME) {}
msg_game::msg_game(u32 s, u32 e) : msg_t(MSG_T_GAME), state(s), ex(e) {}
msg_game::msg_game(u32 s, u32 e, const string& m) : msg_t(MSG_T_GAME), state(s), ex(e), message(m) {}

msg_move::msg_move() : msg_t(MSG_T_MOVE) {}
msg_move::msg_move(u32 xx, u32 yy) : msg_t(MSG_T_MOVE), x(xx), y(yy) {}
msg_move::msg_move(u32 c, u32 xx, u32 yy) : msg_t(MSG_T_MOVE), chess(c), x(xx), y(yy) {}


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
shared_ptr<msg_result> unpack_result(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_result>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_request> unpack_request(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_request>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_reg> unpack_reg(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_reg>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_roomlist> unpack_roomlist(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_roomlist>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_room_oper> unpack_roomoper(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_room_oper>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_room_info> unpack_roominfo(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_room_info>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_chess> unpack_chess(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_chess>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_state> unpack_state(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_state>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_game> unpack_game(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_game>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
shared_ptr<msg_move> unpack_move(const msg_raw_t& raw)
{
    auto ret = make_shared<msg_move>();
    int s = unpack(raw, *ret);
    if(s) return nullptr;
    return ret;
}
