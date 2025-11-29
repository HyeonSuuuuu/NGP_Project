#pragma once
#include "Packet.h"
#include "Timer.h"
class Session;
// 전역변수 =======================================
extern GameTimer g_timer;
extern std::vector<Session*>	g_sessions;
extern CRITICAL_SECTION			g_csSessions;
extern std::atomic<bool>		g_isRunning;
extern HANDLE					g_sendevent;

// 전역상수 =======================================
constexpr int MAX_PLAYER = 64;
constexpr float DIAG = 0.7071f;
// 전역벡터 =======================================
extern std::vector<Obstacle> g_obstacles;
extern std::vector<KillEventPacket> g_killEvents;
extern std::vector<Bullet> g_bullets;
extern std::vector<PlayerInfo> g_players;

// 다렉함수 (필요한 부분만 가져옴) =================
constexpr float XM_PI = 3.141592654f;
constexpr float XMConvertToRadians(float fDegrees) noexcept { return fDegrees * (XM_PI / 180.0f); }
constexpr float XMConvertToDegrees(float fRadians) noexcept { return fRadians * (180.0f / XM_PI); }
// ===============================================

void InitGlobals();
void ReleaseGlobals();

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}