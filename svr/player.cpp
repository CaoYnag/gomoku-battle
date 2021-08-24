#include "player.h"

Player::Player(u64 id, const string& name, const string& ip, u32 port)
	: player_t(id, name, PLAYER_STATE_IDLE, ip, port) {}

Player::~Player() {}
