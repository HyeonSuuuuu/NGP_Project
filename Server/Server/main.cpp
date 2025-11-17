#include "stdafx.h"




int main()
{
	InitGlobals();
	SOCKET listen_sock;
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

	// Accept 스레드 생성
	HANDLE hThread = CreateThread(NULL, 0, AcceptLoop, (LPVOID)listen_sock, 0, NULL);


	// GameLoop
	while (true) {
		g_timer.Tick(30.f);
		// Sessions 루프돌며 RecvEvent HANDLE 배열 만들어서 집어넣음
		// WaitForMultipleObjects << 만든 HANDLE 배열로 동기화
		// KillEvent 비활성화, vector<KillEventPacket> 초기화
		
		// 총알 Update
		// 이동처리
		// 충돌처리
		// hp <= 0 이면 Dead = TRUE, KillEvent Set, vector kill패킷 삽입
		// 스냅샷 복사
		// SendEvent Set
	}


	ReleaseGlobals();
}