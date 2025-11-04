#include "stdafx.h"
#include "Server.h"

ERR_CODE Server::Init()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return ERR_CODE::WSASTARTUP_FAIL;

	m_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listenSock == INVALID_SOCKET)
		return ERR_CODE::SOCK_CREATE_FAIL;
	
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(SERVER_PORT);
	int retVal = bind(m_listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
		return ERR_CODE::SOCK_BIND_FAIL;
	
	retVal = listen(m_listenSock, SOMAXCONN);
	if (retVal == SOCKET_ERROR)
		return ERR_CODE::SOCK_LISTEN_FAIL;
}

DWORD WINAPI Server::AcceptThreadFunc(void* args)
{

	return 0;
}