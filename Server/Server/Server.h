#pragma once
#include "Session.h"
#include "Timer.h"

class Server {
public:

	Server() : m_isRunning(true)
	{}

	ERR_CODE Init();
	DWORD WINAPI AcceptThreadFunc(void* args);
	// TODO : 지형 정보(장애물, 지형크기) 초기화 함수
	// TODO : Accept() // Init이랑 AcceptThread 생성 분리하는게 좋을듯? 생성하고 바로 반환
	// TODO : ERR_CODE 오류 처리 메서드
private:
	SOCKET m_listenSock;
	std::atomic<bool> m_isRunning;
	std::vector<Session> m_sessions;
	CRITICAL_SECTION m_csSession;
	std::vector<Obstacle> m_obstacles;
	HANDLE m_acceptThread;
	GameTimer timer;
};