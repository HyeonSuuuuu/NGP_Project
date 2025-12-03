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
	size_t delay = 0;


	size_t one_count = 0;
	size_t two_count = 0;
	size_t three_count = 0;
};

