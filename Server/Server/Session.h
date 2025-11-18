#pragma once

class Session {
public:
	Session(Session& session)
	{
		// TODO: new Session
	}

private:
	SOCKET socket;
	uint32_t sessionId;
	std::atomic<bool> isConnected;
	PlayerInfo data;
	uint16_t inputflag;
	HANDLE recvEvent;
};