#pragma once
#include "Packet.h"
#include "Timer.h"
class Session;
// 전역변수 =======================================
extern GameTimer g_timer;
extern std::vector<Session*>	g_sessions;
extern CRITICAL_SECTION			g_csSessions;
extern SnapshotPacket			g_snapshots;
extern std::atomic<bool>		g_isRunning;
extern HANDLE					g_sendevent;
// 전역상수 =======================================
constexpr int MAX_PLAYER = 64;
// 전역벡터 =======================================
extern std::vector<Obstacle> g_obstacles;
extern std::vector<KillEventPacket> g_killEvents;



void InitGlobals();
void ReleaseGlobals();

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}