#include "stdafx.h"
#include "Globals.h"




int									g_myId;
std::atomic<uint16_t>				g_inputFlag;
std::vector<Obstacle>				g_obstacles;
std::vector<PlayerInfo>				g_players;
std::vector<Bullet>					g_bullets;
std::vector<KillEventPacket>		g_killEvents;


void InitGlobals()
{
	g_obstacles.reserve(OBSTACLE_COUNT);
}
void ReleaseGlobals()
{

}