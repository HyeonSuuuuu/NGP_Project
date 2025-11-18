#include "stdafx.h"
#include "Globals.h"



// 전역변수 ===========================================
GameTimer g_timer;
std::vector<Session*>	g_sessions;
CRITICAL_SECTION		g_csSessions;
SnapshotPacket			g_snapshots;
std::atomic<bool>		g_isRunning;
HANDLE					g_sendevent;


void InitGlobals()
{
	g_isRunning.store(true);
	InitializeCriticalSection(&g_csSessions);
}

void ReleaseGlobals()
{
	DeleteCriticalSection(&g_csSessions);
}

