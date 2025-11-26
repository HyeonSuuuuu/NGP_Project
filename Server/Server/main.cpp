#include "stdafx.h"
#include "main.h"
#include "Session.h"


int main()
{
	// 지역변수 =====================================
	std::vector<Bullet> bullets;
	std::vector<Obstacle> obstacles;
	std::array<HANDLE, MAX_PLAYER> recvEvents;
	// =============================================
	obstacles.clear();
	obstacles.reserve(10);
	for (int i = 0; i < 10; ++i)
	{
		Obstacle obs{};
		obs.size = 4.0f;
		obs.x = RandF(-50.f, 50.f);
		obs.z = RandF(10.f, 90.f);
		obstacles.push_back(obs);
	}
	g_obstacles = obstacles;
	SOCKET listen_sock = NULL;
	InitGlobals();
	InitNetwork(listen_sock);
	HANDLE hThread = CreateThread(NULL, 0, AcceptThread, (LPVOID)listen_sock, 0, NULL);
	// GameLoop
	while (g_isRunning.load()) {
		g_timer.Tick(30.f);
		
		WaitAllRecvEvent(recvEvents);
		// TODO: Send Event 초기화, KillEvent Vector 초기화
		ResetEvent(g_sendevent);
		EnterCriticalSection(&g_csSessions);
		g_killEvents.clear();
		LeaveCriticalSection(&g_csSessions);
		// TODO: 총알 Update
		{
			float fElapsedTime = g_timer.GetTimeElapsed();
			const float fWorldBound = (float)WORLD_SIZE / 2.0f;

			for (auto it = bullets.begin(); it != bullets.end(); ) {
				Bullet& bullet = *it;

				float distance = bullet.speed * fElapsedTime;

				float yawRad = bullet.yawAngle * (float)M_PI / 180.0f;

				bullet.x += distance * sin(yawRad); // X-movement
				bullet.z += distance * cos(yawRad); // Z-movement

				if (bullet.x < -fWorldBound || bullet.x > fWorldBound ||
					bullet.z < -fWorldBound || bullet.z > fWorldBound)
				{
					it = bullets.erase(it);
				}
				else {
					++it;
				}
			}
		}
		// TODO: 모든 Session Update
		// TODO: 충돌처리
		// TODO: hp <= 0이라면 KillEventPacket 생성, vector에 Push(전역변수)
		// TODO: 스냅샷 Update(전역변수)
		// TODO: SendEvent Set(전역변수)
	}
	// TODO: 모든 스레드 Join
	ReleaseGlobals();
	WSACleanup();
}

void InitNetwork(SOCKET listen_sock)
{
	int retval;
	WSADATA wsa;
	retval = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (retval != 0)
		err_display(retval);

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("listen socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(SERVER_PORT);
	int retVal = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retVal == SOCKET_ERROR)
		err_quit("bind()");

	retVal = listen(listen_sock, SOMAXCONN);
	if (retVal == SOCKET_ERROR)
		err_quit("listen()");
}

void WaitAllRecvEvent(std::array<HANDLE, MAX_PLAYER>& arr)
{
	// recvEvent 배열 생성
	EnterCriticalSection(&g_csSessions);
	const int size = g_sessions.size();
	for (int i = 0; i < size; ++i) { 
		arr[i] = g_sessions[i]->recvEvent;
	}
	LeaveCriticalSection(&g_csSessions);
	// 모두 깨어날 때까지 대기
	WaitForMultipleObjects(size, arr.data(), true, INFINITE); 
}

