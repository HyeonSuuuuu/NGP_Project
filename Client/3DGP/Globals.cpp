#include "stdafx.h"
#include "Globals.h"



char								g_ipAddress[16];
int									g_myId;
std::atomic<uint8_t>				g_inputFlag;
std::atomic<uint16_t>				g_yawAngle;

std::vector<Obstacle>				g_obstacles;
std::vector<PlayerInfo>				g_players;
std::vector<Bullet>					g_bullets;
std::vector<KillEvent>				g_killEvents;

CRITICAL_SECTION					g_csPlayers;
CRITICAL_SECTION					g_csBullets;
CRITICAL_SECTION					g_csKillEvents;
HANDLE								g_enterEvent;

// 메인로직에서만 접근
int									g_enemyCount;

void InitGlobals()
{
	InitializeCriticalSection(&g_csPlayers);
	InitializeCriticalSection(&g_csBullets);
	InitializeCriticalSection(&g_csKillEvents);
	g_enterEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}
void ReleaseGlobals()
{
	DeleteCriticalSection(&g_csPlayers);
	DeleteCriticalSection(&g_csBullets);
	DeleteCriticalSection(&g_csKillEvents);
	CloseHandle(g_enterEvent);
}

void TextOutEx(HDC hDC, int x, int y, const char* fmt, ...)
{
	char buffer[512];

	va_list args;
	va_start(args, fmt);
	vsprintf_s(buffer, fmt, args);
	va_end(args);

	TextOutA(hDC, x, y, buffer, lstrlenA(buffer));
}