#pragma once
#include "../../Server/Server/Common.h"
#include "../../Server/Server/Packet.h"

struct KillEvent {
	int32_t killerId;
	int32_t killedId;
	int16_t displayTime;
};

extern char							g_ipAddress[16];
extern int							g_myId;
extern std::atomic<uint8_t>			g_inputFlag;
extern std::atomic<uint16_t>		g_yawAngle;
extern std::vector<Obstacle>		g_obstacles;
extern std::vector<PlayerInfo>		g_players;
extern std::vector<Bullet>			g_bullets;
extern std::vector<KillEvent> g_killEvents;

extern CRITICAL_SECTION					g_csPlayers;
extern CRITICAL_SECTION					g_csBullets;
extern CRITICAL_SECTION					g_csKillEvents;
extern HANDLE							g_enterEvent;

extern int								g_enemyCount;
void InitGlobals();
void ReleaseGlobals();


void TextOutEx(HDC hDC, int x, int y, const char* fmt, ...);