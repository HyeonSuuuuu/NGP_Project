#pragma once
#include "../../Server/Server/Common.h"
#include "../../Server/Server/Packet.h"


extern int							g_myId;
extern std::atomic<uint8_t>			g_inputFlag;
extern std::atomic<uint16_t>		g_yawAngle;
extern std::vector<Obstacle>		g_obstacles;
extern std::vector<PlayerInfo>		g_players;
extern std::vector<Bullet>			g_bullets;
extern std::vector<KillEventPacket> g_killEvents;

extern CRITICAL_SECTION					g_csPlayers;
extern CRITICAL_SECTION					g_csBullets;
extern CRITICAL_SECTION					g_csKillEvents;
extern HANDLE							g_enterEvent;

extern int								g_enemyCount;
void InitGlobals();
void ReleaseGlobals();

void TextOutEx(HDC hDC, int x, int y, const char* fmt, ...);