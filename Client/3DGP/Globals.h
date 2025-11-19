#pragma once
#include "../../Server/Server/Common.h"
#include "../../Server/Server/Packet.h"


extern int							g_myId;
extern std::atomic<uint16_t>		g_inputFlag;
extern std::vector<Obstacle>		g_obstacles;
extern std::vector<PlayerInfo>		g_players;
extern std::vector<Bullet>			g_bullets;
extern std::vector<KillEventPacket> g_killEvents;

void InitGlobals();
void ReleaseGlobals();