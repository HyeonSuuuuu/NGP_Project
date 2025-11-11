#pragma once
#include "SessionManager.h"
#include "Timer.h"

class Server {
public:

	Server() : m_isRunning(true)
	{}

	ERR_CODE Init();
	DWORD WINAPI AcceptThreadFunc(void* args);
	// TODO : 지형 정보(장애물, 지형크기) 초기화 함수
	// TODO : ERR_CODE 오류 처리 메서드
private:
	SOCKET m_listenSock;
	std::atomic<bool> m_isRunning;
	SessionManager m_sessionManager;
	std::vector<Obstacle> m_obstacles;
	HANDLE m_acceptThread;
	GameTimer timer;
};