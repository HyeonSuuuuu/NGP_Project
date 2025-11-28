#include "stdafx.h"
#include "NetworkThread.h"
#include "Timer.h"
#include "Globals.h"
#include "../../Server/Server/Common.h"
#include "../../Server/Server/Packet.h"

int count;

DWORD WINAPI NetworkThread(void* args)
{
	char buf[65536];
	SOCKET sock = NULL;
	ConnectServer(sock);
	PacketHeader header;
	recv(sock, reinterpret_cast<char*>(&header), sizeof(PacketHeader), MSG_WAITALL);
	if (header.type == SC_ENTER) {
		recv(sock, buf, header.size, MSG_WAITALL);
		ProcessEnterPacket(buf);
	}

	CGameTimer timer;
	while (true) {
		count++;
		timer.Tick(30.f);
		SendInputPacket(sock);

		recv(sock, reinterpret_cast<char*>(&header), sizeof(PacketHeader), MSG_WAITALL);
		if (header.type == SC_SNAPSHOT) {
			recv(sock, buf, header.size, MSG_WAITALL);
			ProcessSnapshotPacket(buf);
		}
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

void ProcessEnterPacket(char* buf)
{
	EnterPacket* enterPacket = reinterpret_cast<EnterPacket*>(buf);
	uint32_t offset = sizeof(EnterPacket);

	g_myId = enterPacket->id;
	int obstaclesCount = enterPacket->obstacleCount;
	g_obstacles.resize(obstaclesCount);
	for (int i = 0; i < obstaclesCount; ++i) {
		g_obstacles[i] = *reinterpret_cast<Obstacle*>(buf + offset);
		offset += sizeof(Obstacle);
	}
	SetEvent(g_enterEvent);
}

void SendInputPacket(SOCKET sock)
{
	PacketHeader header;
	header.type = CS_INPUT;
	header.size = sizeof(InputPacket);
	InputPacket inputPkt;
	inputPkt.id = g_myId;
	inputPkt.inputFlag = g_inputFlag.load();
	inputPkt.yawAngle = g_yawAngle.load();
	send(sock, reinterpret_cast<char*>(&header), sizeof(PacketHeader), 0);
	send(sock, reinterpret_cast<char*>(&inputPkt), sizeof(InputPacket), 0);
}


void ProcessSnapshotPacket(char* buf)
{
	SnapshotPacket* snapshotPacket = reinterpret_cast<SnapshotPacket*>(buf);
	uint32_t offset = sizeof(SnapshotPacket);

	if (g_players.size() != snapshotPacket->playerCount) {
		g_players.resize(snapshotPacket->playerCount);
	}
		
	EnterCriticalSection(&g_csPlayers);
	for (int i = 0; i < snapshotPacket->playerCount; ++i) {
		PlayerInfo* playerInfo = reinterpret_cast<PlayerInfo*>(buf + offset);
		offset += sizeof(PlayerInfo);

		g_players[i] = *playerInfo;
		if (!(count % 30))
			DebugLog("(%f, %f)\n", playerInfo->x, playerInfo->z);
	}
	LeaveCriticalSection(&g_csPlayers);
}