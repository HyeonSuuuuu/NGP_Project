#pragma once

class Session {
public:
	DWORD WINAPI SessionThreadFunc(void*);

private:
	SOCKET m_socket;
	std::atomic<bool> m_isConnected;
	PlayerInfo m_playerInfo;
	HANDLE m_sessionThread;
};


