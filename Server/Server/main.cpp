#include "stdafx.h"
#include "main.h"
#include "Session.h"


int main()
{
	// 초기화 =====================================
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
		{
			constexpr float PLAYER_SPEED = 10.0f;
			float fElapsedTime = g_timer.GetTimeElapsed();
			const float fWorldBound = (float)WORLD_SIZE / 2.0f;

			EnterCriticalSection(&g_csSessions);
			for (Session* session : g_sessions)
			{
				if (!session->isConnected.load() || session->data.isDead) continue;

				PlayerInfo& player = session->data;
				uint16_t input = session->inputflag;

				float moveX = 0.0f;
				float moveZ = 0.0f;
				bool isMoving = false;

				if (input & INPUT_MOVE_FORWARD) { moveZ += 1.0f; isMoving = true; }
				if (input & INPUT_MOVE_BACKWARD) { moveZ -= 1.0f; isMoving = true; }
				if (input & INPUT_MOVE_LEFT) { moveX -= 1.0f; isMoving = true; }
				if (input & INPUT_MOVE_RIGHT) { moveX += 1.0f; isMoving = true; }

				if (isMoving) {
					float length = sqrt(moveX * moveX + moveZ * moveZ);
					moveX /= length;
					moveZ /= length;

					float distance = PLAYER_SPEED * fElapsedTime;
					player.x += moveX * distance;
					player.z += moveZ * distance;
				}
				
				// World Boundary Check
				player.x = max(-fWorldBound, min(fWorldBound, player.x));
				player.z = max(-fWorldBound, min(fWorldBound, player.z));

				if (input & INPUT_FIRE)
				{
					Bullet newBullet{};
					newBullet.ownerId = player.id;
					newBullet.x = player.x;
					newBullet.z = player.z;
					newBullet.yawAngle = player.yawAngle;
					newBullet.speed = 30.0f; // TODO: move to constant
					bullets.push_back(newBullet);
				}

				// Reset input flag after processing
				session->inputflag = 0;
			}
			LeaveCriticalSection(&g_csSessions);
		}
		// TODO: 충돌처리
		// TODO: hp <= 0이면 KillEventPacket 생성, vector에 Push(임계구역)
		// TODO: 스냅샷 Update(임계구역)
		// TODO: SendEvent Set(임계구역)
	}
	// TODO: 모든 스레드 Join
	ReleaseGlobals();
	WSACleanup();
}


void InitNetwork(SOCKET* listen_sock)
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
	// recvEvent 배열 갱신
	EnterCriticalSection(&g_csSessions);
	const int size = g_sessions.size();
	for (int i = 0; i < size; ++i) { 
		arr[i] = g_sessions[i]->recvEvent;
	}
	LeaveCriticalSection(&g_csSessions);
	// 모든 네크워크 스레드가 시그널 상태가 되기를 기다림
	WaitForMultipleObjects(size, arr.data(), true, INFINITE); 
}
