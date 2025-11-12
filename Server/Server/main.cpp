#include "stdafx.h"




int main()
{
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

	while (true) {

	}
}