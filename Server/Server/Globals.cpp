#include "stdafx.h"
#include "Globals.h"



// Àü¿ªº¯¼ö ===========================================
GameTimer g_timer;
std::vector<Session*>	g_sessions;
CRITICAL_SECTION		g_csSessions;
std::atomic<bool>		g_isRunning;
HANDLE					g_sendevent;

std::vector<KillEventPacket> g_killEvents;
std::vector<Obstacle>	g_obstacles;
std::vector<Bullet>		g_bullets;


void InitGlobals()
{
	g_isRunning.store(true);
	InitializeCriticalSection(&g_csSessions);
}

void ReleaseGlobals()
{
	DeleteCriticalSection(&g_csSessions);
}

