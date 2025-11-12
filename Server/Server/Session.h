#pragma once

struct Session {
	SOCKET socket;
	std::atomic<bool> isConnected;
	PlayerInfo info;
	std::atomic<uint8_t> inputflag;

	HANDLE thread;
	CRITICAL_SECTION cs;
};