#pragma once

class Session {
public:
	DWORD WINAPI NetworkThreadFunc(void* args);

private:
	SOCKET m_socket;
	std::atomic<bool> m_isConnected;
	PlayerInfo m_playerInfo;
	std::atomic<uint8_t> m_inputFlag;

	HANDLE m_NetworkThread;
	CRITICAL_SECTION m_csSession;
};
