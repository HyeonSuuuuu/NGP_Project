#include "stdafx.h"
#include "main.h"
#include "Session.h"

int main()
{
	std::cout << "hello";
	// 지역변수 =====================================
	std::array<HANDLE, MAX_PLAYER> recvEvents;
	size_t tickCount = 0;
	// =============================================

	g_obstacles.reserve(10);
	for (int i = 0; i < 1; ++i)
	{
		Obstacle obs{};
		obs.size = 4.0f;
		obs.x = RandF(-50.f, 50.f);
		obs.z = RandF(10.f, 90.f);
		g_obstacles.push_back(obs);
	}

	SOCKET listen_sock = NULL;
	InitGlobals();
	InitNetwork(listen_sock);
	HANDLE hThread = CreateThread(NULL, 0, AcceptThread, (LPVOID)listen_sock, 0, NULL);
	// GameLoop
	while (g_isRunning.load()) {
		g_timer.Tick(60.f);
		if (!(tickCount % 60))
			std::cout << g_timer.GetFrameRate();
		WaitAllRecvEvent(recvEvents);
		// TODO: Send Event 초기화, KillEvent Vector 초기화
		ResetEvent(g_sendevent);
		g_killEvents.clear();
		g_players.clear();

		// TODO: 모든 Session Update
		EnterCriticalSection(&g_csSessions);
		const float moveStep = 0.2f;
		for (Session* session : g_sessions) {
			if (session->data.isDead) {
				// 리스폰 처리 30 = 1초,180 = 6초
				if (tickCount - session->delay > 30) {
					session->data.isDead = false;
					session->data.hp = session->data.maxHp;
					session->data.x = RandF(-50.f, 50.f);
					session->data.z = RandF(-10.f, 90.f);
				}
				continue;
			}
			
			// 상점 1: 공격력 증가, 2: 최대체력 증가, 3: 체력회복
			if (session->inputflag & INP_ONE) {
				if (session->data.gold >= 200) {
					session->data.atk += 10;
					session->data.gold -= 200;
				}
			}
			if (session->inputflag & INP_TWO) {
				if (session->data.gold >= 300) {
					session->data.maxHp += 20;
					session->data.hp += 20;
					session->data.gold -= 300;
				}
			}
			if (session->inputflag & INP_THREE) {
				if (session->data.gold >= 100) {
					session->data.hp += session->data.maxHp;
					session->data.gold -= 100;
				}
			}

			// 이동, 회전 처리
			float yawRad = XMConvertToRadians(session->data.yawAngle);

			float forwardX = sinf(yawRad);
			float forwardZ = cosf(yawRad);

			float rightX = cosf(yawRad);
			float rightZ = -sinf(yawRad);

			bool forwardFlag = false;
			bool rightFlag = false;

			float dx = 0, dz = 0;

			if (session->inputflag & INP_FORWARD) {
				forwardFlag = true;
				dx += moveStep * forwardX;
				dz += moveStep * forwardZ;
			}
			if (session->inputflag & INP_BACKWARD) {
				forwardFlag = true;
				dx -= moveStep * forwardX;
				dz -= moveStep * forwardZ;
			}
			if (session->inputflag & INP_LEFT) {
				rightFlag = true;
				dx -= moveStep * rightX;
				dz -= moveStep * rightZ;
			}
			if (session->inputflag & INP_RIGHT) {
				rightFlag = true;
				dx += moveStep * rightX;
				dz += moveStep * rightZ;
			}
			if (forwardFlag && rightFlag) {
				dx *= DIAG;
				dz *=  DIAG;
			}
			session->prevX = session->data.x;
			session->prevZ = session->data.z;

			session->data.x += dx;
			session->data.z += dz;

			// 총알 처리
			if (session->inputflag & INP_SPACEBAR) {
				if (tickCount - session->delay > 60) {
					g_bullets.emplace_back(Bullet{ session->data.x, session->data.z,
					session->data.yawAngle, 1.0f * 5, (int)session->data.id });
					session->delay = tickCount;
				}
			}
		}
		LeaveCriticalSection(&g_csSessions);
		
		// 총알 Update
		for (Bullet& bullet : g_bullets) {
			float yawRad = XMConvertToRadians(bullet.yawAngle);
			bullet.x += bullet.speed * sinf(yawRad); // X-movement
			bullet.z += bullet.speed * cosf(yawRad); // Z-movement
		}

		// 범위 처리 (월드 밖)
		// width 100 depth 100 x 0 z 40 (-50, 50), (-10, 90)
		EnterCriticalSection(&g_csSessions);
		for (Session* session : g_sessions) {
			if (session->data.x < -50.f || session->data.x > 50.f ||
				session->data.z < -10.f || session->data.z > 90.f)
			{
				session->data.x = session->prevX;
				session->data.z = session->prevZ;
			}
		}
		LeaveCriticalSection(&g_csSessions);

		for (auto it = g_bullets.begin(); it != g_bullets.end(); ) {
			Bullet& bullet = *it;
			if (bullet.x < -50.f || bullet.x > 50.f ||
				bullet.z < -10.f || bullet.z > 90.f)
			{
				it = g_bullets.erase(it);
			}
			else {
				++it;
			}
		}

		// 충돌 처리
		EnterCriticalSection(&g_csSessions);
		for (Session* session : g_sessions) {
			if (session->data.isDead)
				continue;
			// 플레이어 - 장애물
			for (const Obstacle& obs : g_obstacles) {
				RECT r1 {session->data.x - 3, session->data.z - 3,
						 session->data.x + 3, session->data.z + 3 };
				RECT r2{ obs.x - obs.size / 2, obs.z - obs.size / 2,
					obs.x + obs.size / 2, obs.z + obs.size / 2 };
				
				RECT result;

				if (IntersectRect(&result, &r1, &r2)) {
					session->data.x = session->prevX;
					session->data.z = session->prevZ;
				}
			}
			// 플레이어 - 총알
			for (auto it = g_bullets.begin(); it != g_bullets.end();) {
				Bullet& bullet = *it;
				RECT r1{ session->data.x - 3, session->data.z - 3,
						 session->data.x + 3, session->data.z + 3 };
				RECT r2{ bullet.x - 0.5f, bullet.z - 0.5f,
						 bullet.x + 0.5f, bullet.z + 0.5f };
				RECT result;
				if (IntersectRect(&result, &r1, &r2)) {
					// 맞춘 플레이어 찾기
					PlayerInfo* atker = nullptr;
					for (Session* s : g_sessions) {
						if (s->sessionId == bullet.ownerId) {
							atker = &s->data;
							break;
						}
					}
					
					// 데미지 처리
					session->data.hp -= atker->atk;
					// 사망처리
					if (session->data.hp <= 0) {
						session->data.isDead = true;
						session->data.deathCount += 1;
						session->delay = tickCount;
						atker->killCount += 1;
						atker->gold += 100;
						// KillEvent 추가
						KillEventPacket packet{ atker->id, session->data.id };
						g_killEvents.emplace_back(packet);
					}
					it = g_bullets.erase(it);
				}
				else {
					it++;
				}
			}
		}
		LeaveCriticalSection(&g_csSessions);

		for (const Obstacle& obs : g_obstacles) {
			for (auto it = g_bullets.begin(); it != g_bullets.end(); ) {
				Bullet& bullet = *it;
				RECT r1{ bullet.x - 0.5f, bullet.z - 0.5f,
						 bullet.x + 0.5f, bullet.z + 0.5f };
				RECT r2{ obs.x - obs.size / 2, obs.z - obs.size / 2,
						 obs.x + obs.size / 2, obs.z + obs.size / 2 };
				RECT result;
				if (IntersectRect(&result, &r1, &r2)) {
					it = g_bullets.erase(it);
				}
				else {
					++it;
				}
			}
		}

		// TODO: 스냅샷 Update(전역변수)
		EnterCriticalSection(&g_csSessions);
		for (Session* sessions : g_sessions) {
			g_players.push_back(sessions->data);
		}
		LeaveCriticalSection(&g_csSessions);
		// TODO: SendEvent Set(전역변수)
		SetEvent(g_sendevent);
		tickCount++;
	}
	// TODO: 모든 스레드 Join
	ReleaseGlobals();
	WSACleanup();
}


void InitNetwork(SOCKET& listen_sock)
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
	WaitForMultipleObjects(size, arr.data(), TRUE, INFINITE); 
}

