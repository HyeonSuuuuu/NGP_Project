#include "stdafx.h"
#include "Globals.h"




int									g_myId;
std::atomic<uint16_t>				g_inputFlag;
std::vector<Obstacle>				g_obstacles;
std::vector<PlayerInfo>				g_players;
std::vector<Bullet>					g_bullets;
std::vector<KillEventPacket>		g_killEvents;

CRITICAL_SECTION					g_csPlayers;
CRITICAL_SECTION					g_csBullets;
CRITICAL_SECTION					g_csKillEvents;
HANDLE								g_enterEvent;

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