#pragma once
#include "Packet.h"
class Session;
// 전역변수 =======================================
extern GameTimer g_timer;
extern std::vector<Session*>	g_sessions;
extern CRITICAL_SECTION			g_csSessions;
extern SnapshotPacket			g_snapshots;
extern std::atomic<bool>		g_isRunning;
extern HANDLE					g_sendevent;



void InitGlobals();
void ReleaseGlobals();
// 상수 ===========================================
#define SERVER_PORT 9000