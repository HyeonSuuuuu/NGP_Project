#pragma once

struct Session {
	SOCKET socket;
	uint32_t session_id;
	std::atomic<bool> isConnected;

	float x, y;
	float yawAngle;

	uint16_t hp;
	int32_t gold;
	bool isDead;

	std::atomic<uint8_t> inputflag;
	HANDLE recv_complete_event;
	HANDLE respone_event;
	CRITICAL_SECTION cs;
};