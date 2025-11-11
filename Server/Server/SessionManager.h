#pragma once
#include "Session.h"

class SessionManager {
public:
	AddSession();
	RemoveSession();
	Broadcast(void* pkt, int size);

private:
	std::vector<Session> csSession;
	CRITICAL_SECTION csSession;

};