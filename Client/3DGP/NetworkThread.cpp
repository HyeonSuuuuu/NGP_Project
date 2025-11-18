#include "stdafx.h"
#include "NetworkThread.h"
#include "Timer.h"

DWORD WINAPI NetworkThread(void* args)
{
	// Connect
	// Recv (SC_ENTER)
	// SC_ENTER 패킷 처리

	CGameTimer timer;
	while (true) {
		timer.Tick(30.f);
		// 
	}
}