#include "stdafx.h"
#include "NetworkThread.h"
#include "Timer.h"
#include "Globals.h"
#include "../../Server/Server/Common.h"
#include "../../Server/Server/Packet.h"

DWORD WINAPI NetworkThread(void* args)
{
	char buf[65536];
	int offset = 0;
	SOCKET sock = NULL;
	ConnectServer(sock);
	PacketHeader header;
	recv(sock, reinterpret_cast<char*>(&header), sizeof(PacketHeader), MSG_WAITALL);
	if (header.type == SC_ENTER) {
		// recv 받았다고 가정 (테스트 안해봄)
		recv(sock, buf, header.size - sizeof(PacketHeader), MSG_WAITALL);
		EnterPacket* enterPacket = reinterpret_cast<EnterPacket*>(buf);
		offset += sizeof(EnterPacket);

		g_myId = enterPacket->id;
		int obstaclesCount = enterPacket->obstacleCount;
		g_obstacles.resize(obstaclesCount);
		for (int i = 0; i < obstaclesCount; ++i) {
			g_obstacles[i] = *reinterpret_cast<Obstacle*>(buf + offset);
			offset += sizeof(Obstacle);
			OutputDebugString(L"%d", offset);
		}
	}
	
	// Recv (SC_ENTER)
	// SC_ENTER 패킷 처리

	CGameTimer timer;
	while (true) {
		timer.Tick(30.f);
		// InputPacket 생성
		// Send (CS_INPUT)
		// Recv 
	}
}


void ConnectServer(SOCKET& sock)
{
	int retval;
	WSADATA wsa;

	retval = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (retval != 0) {
		err_display(retval);
		exit(1);
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
		err_quit("connect()");
}