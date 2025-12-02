// AcceptThread.cpp
#include "stdafx.h"
#include "Thread.h"
#include "Session.h"
#include "Packet.h"
#include "Common.h"        // g_sessions, g_isRunning, g_csSessions 등 전역 변수 여기 있음
#include <ws2tcpip.h>
#include <iostream>
#include "Globals.h"


DWORD WINAPI AcceptThread(void* args)
{
    SOCKET listen_sock = reinterpret_cast<SOCKET>(args);
    uint32_t idCount = 1000;   // 플레이어 ID 시작 번호

    std::cout << "[AcceptThread] 생성 성공 - 클라이언트 접속 대기 중..." << std::endl;

    while (g_isRunning.load())
    {
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);

        SOCKET clientSock = accept(listen_sock, (sockaddr*)&clientAddr, &addrLen);
        if (clientSock == INVALID_SOCKET)
        {
            if (g_isRunning.load())
                std::cout << "[Accept] accept() 실패: " << WSAGetLastError() << std::endl;
            continue;
        }

        EnterCriticalSection(&g_csSessions);
        if (g_sessions.size() >= MAX_PLAYERS) {
            std::cout << "클라이언트 접속 거부: 인원 초과" << std::endl;
            closesocket(clientSock);
            continue;
        }
        LeaveCriticalSection(&g_csSessions);

        // 클라이언트 IP 출력
        char ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
        std::cout << "[접속] 클라이언트 접속 → " << ip << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // Session 동적 생성
        Session* newSession = new Session();
        newSession->socket = clientSock;
        newSession->sessionId = idCount++;
        newSession->isConnected.store(true);
        newSession->inputflag = 0;

        // 플레이어 초기 위치 (맵 범위 맞춰서)
        newSession->data.id = newSession->sessionId;
        newSession->data.x = RandF(-45.0f, 45.0f);
        newSession->data.z = RandF(-5.0f, 85.0f);
        newSession->data.atk = 100;
        newSession->data.yawAngle = 0;
        newSession->data.hp = 100;
        newSession->data.gold = 0;
        newSession->data.isDead = false;

        // NetworkThread 생성 (각 클라이언트마다 하나씩)
        HANDLE hThread = CreateThread(nullptr, 0, NetworkThread, newSession, 0, nullptr);
        if (hThread == nullptr)
        {
            std::cout << "[ERROR] NetworkThread 생성 실패" << std::endl;
            closesocket(clientSock);
            delete newSession;
            continue;
        }
    }
    
    std::cout << "[AcceptThread] 종료" << std::endl;
    return 0;
}