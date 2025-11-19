// AcceptThread.cpp
#include "stdafx.h"
#include "Thread.h"
#include "Session.h"
#include "Packet.h"
#include "Common.h"        // g_sessions, g_isRunning, g_csSessions 등 전역 변수 여기 있음
#include <ws2tcpip.h>
#include <iostream>

extern std::vector<Session*> g_sessions;           // main.cpp나 Globals.cpp에 선언된 전역
extern std::atomic<bool> g_isRunning;
extern CRITICAL_SECTION g_csSessions;
extern std::vector<Obstacle> g_obstacles;          // main에서 만든 장애물 벡터

DWORD WINAPI AcceptThread(void* args)
{
    SOCKET listen_sock = reinterpret_cast<SOCKET>(args);
    uint32_t idCount = 1000;   // 플레이어 ID 시작 번호

    printf("[AcceptThread] 생성 성공 - 클라이언트 접속 대기 중...\n");

    while (g_isRunning.load())
    {
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);

        SOCKET clientSock = accept(listen_sock, (sockaddr*)&clientAddr, &addrLen);
        if (clientSock == INVALID_SOCKET)
        {
            if (g_isRunning.load())
                printf("[Accept] accept() 실패: %d\n", WSAGetLastError());
            continue;
        }

        // 클라이언트 IP 출력
        char ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
        printf("[접속] 클라이언트 접속 → %s:%d\n", ip, ntohs(clientAddr.sin_port));

        // Session 동적 생성
        Session* newSession = new Session();
        newSession->socket = clientSock;
        newSession->sessionId = idCount++;
        newSession->isConnected.store(true);
        newSession->inputflag = 0;

        // WSAEvent 생성 (비동기 Recv용)
        newSession->recvEvent = WSACreateEvent();
        WSAEventSelect(clientSock, newSession->recvEvent, FD_READ | FD_CLOSE);

        // 플레이어 초기 위치 (맵 범위 맞춰서)
        newSession->data.id = newSession->sessionId;
        newSession->data.x = RandF(-45.0f, 45.0f);
        newSession->data.z = RandF(15.0f, 85.0f);
        newSession->data.yawAngle = 0;
        newSession->data.hp = 100;
        newSession->data.gold = 500;
        newSession->data.isDead = false;

        // 전역 세션 리스트에 추가 (스레드 세이프)
        EnterCriticalSection(&g_csSessions);
        g_sessions.push_back(newSession);
        LeaveCriticalSection(&g_csSessions);

        // NetworkThread 생성 (각 클라이언트마다 하나씩)
        HANDLE hThread = CreateThread(nullptr, 0, NetworkThread, newSession, 0, nullptr);
        if (hThread == nullptr)
        {
            printf("[ERROR] NetworkThread 생성 실패\n");
            closesocket(clientSock);
            delete newSession;
            continue;
        }

        // 접속 완료 후 바로 SC_ENTER 패킷 전송 (장애물 동기화)
        {
            PacketHeader header{};
            header.type = SC_ENTER;
            header.size = sizeof(PacketHeader) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(Obstacle) * g_obstacles.size();

            EnterPacket enterPkt{};
            enterPkt.id = newSession->sessionId;
            enterPkt.obstacleCount = static_cast<uint16_t>(g_obstacles.size());
            enterPkt.obstacles = g_obstacles.data();

            // 임시 버퍼에 패킷 조립
            char sendBuf[1024]{};
            memcpy(sendBuf, &header, sizeof(header));
            memcpy(sendBuf + sizeof(header), &enterPkt.id, sizeof(uint32_t));
            memcpy(sendBuf + sizeof(header) + sizeof(uint32_t), &enterPkt.obstacleCount, sizeof(uint16_t));
            memcpy(sendBuf + sizeof(header) + sizeof(uint32_t) + sizeof(uint16_t),
                enterPkt.obstacles, sizeof(Obstacle) * g_obstacles.size());

            send(clientSock, sendBuf, header.size, 0);

            printf("[패킷 전송] SC_ENTER → Player %u (장애물 %zu개 동기화)\n",
                newSession->sessionId, g_obstacles.size());
        }

        printf("[성공] Player %u 접속 완료 → 현재 인원: %zu명\n",
            newSession->sessionId, g_sessions.size());
    }

    printf("[AcceptThread] 종료\n");
    return 0;
}