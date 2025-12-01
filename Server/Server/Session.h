#pragma once

struct Session {
	SOCKET socket;
	uint32_t sessionId;
	std::atomic<bool> isConnected;
	PlayerInfo data;
	uint16_t inputflag;
	HANDLE recvEvent;

	float prevX = 0.f;
	float prevZ = 0.f;
};

