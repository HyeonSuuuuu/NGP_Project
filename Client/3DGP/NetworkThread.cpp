#include "stdafx.h"
#include "NetworkThread.h"
#include "Timer.h"
#include "../../Server/Server/Common.h"
#include "../../Server/Server/Packet.h"

DWORD WINAPI NetworkThread(void* args)
{
	// Connect
	// Recv (SC_ENTER)
	// SC_ENTER 菩哦 贸府

	CGameTimer timer;
	while (true) {
		timer.Tick(30.f);
		// InputPacket 积己
		// Send (CS_INPUT)
		// Recv
	}
}