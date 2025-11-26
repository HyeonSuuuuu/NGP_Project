#include "stdafx.h"
#include "Thread.h"
#include "Session.h"
#include "Packet.h"
#include "Globals.h"
#include <iostream>
#include <vector>

extern CRITICAL_SECTION g_csSessions;
extern SnapshotPacket g_snapshots;
extern HANDLE g_sendevent;

// 패킷 수신 처리 함수
void ProcessPacket(Session& session, const PacketHeader* header)
{
    if (header->type == CS_INPUT) {
        const InputPacket* inputPkt = reinterpret_cast<const InputPacket*>(header + 1);

        session.inputflag = inputPkt->inputFlag;
        session.data.yawAngle = static_cast<float>(inputPkt->yawAngle);
        SetEvent(session.recvEvent);
    }
}

// 클라이언트 연결 해제 처리
void DisconnectSession(Session& session)
{

    WSAEventSelect(session.socket, NULL, 0);
    closesocket(session.socket);

    session.isConnected.store(false);

    printf("[연결 해제] Player %u 연결 종료\n", session.sessionId);
}

int HandleRecv(Session& session, char* buffer, const int bufferSize)
{
    // 패킷 헤더받기
    int nRecv = recv(session.socket, buffer, sizeof(PacketHeader), MSG_PEEK);

    if (nRecv == 0) return 0;
    if (nRecv == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) return 1;
        return -1;
    }

    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);

    if (header->size > bufferSize) {
        printf("[ERROR] 패킷 크기 초과: %u\n", header->size);
        return -1;
    }

    // 패킷 전체 받기
    nRecv = recv(session.socket, buffer, header->size, 0);

    if (nRecv == header->size) {
        ProcessPacket(session, header);
        return 1;
    }

    return -1;
}

DWORD WINAPI NetworkThread(void* args)
{
    Session& session = *reinterpret_cast<Session*>(args);

    // Recv, Send 버퍼 정의
    constexpr int MAX_BUFFER_SIZE = 1024 * 4;
    char recvBuffer[MAX_BUFFER_SIZE]{};
    char sendBuffer[MAX_BUFFER_SIZE]{};

    HANDLE waitHandles[2] = { session.recvEvent, g_sendevent };

    while (session.isConnected.load()) {

        DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {

            WSANETWORKEVENTS netEvents;
            WSAEnumNetworkEvents(session.socket, session.recvEvent, &netEvents);

            if (netEvents.lNetworkEvents & FD_READ) {
                int result = HandleRecv(session, recvBuffer, MAX_BUFFER_SIZE);
                if (result <= 0) {
                    if (result == 0) printf("[Recv] 클라이언트(%u) 연결 종료 요청 감지\n", session.sessionId);
                    else printf("[Recv] 클라이언트(%u) 수신 에러: %d\n", session.sessionId, netEvents.iErrorCode[FD_READ_BIT]);
                    DisconnectSession(session);
                    continue;
                }
            }
            if (netEvents.lNetworkEvents & FD_CLOSE) {
                printf("[Close] 클라이언트(%u) 연결 끊김 감지: %d\n", session.sessionId, netEvents.iErrorCode[FD_CLOSE_BIT]);
                DisconnectSession(session);
                continue;
            }

        }
        else if (waitResult == (WAIT_OBJECT_0 + 1)) {
            EnterCriticalSection(&g_csSessions);
            if (!g_killEvents.empty())
            {
                PacketHeader killHeader{};
                killHeader.type = SC_KILL_EVENT;
                killHeader.size = sizeof(PacketHeader) +
                    sizeof(KillEventPacket) * static_cast<uint16_t>(g_killEvents.size());

                memcpy(sendBuffer, &killHeader, sizeof(killHeader));
                memcpy(sendBuffer + sizeof(PacketHeader),
                    g_killEvents.data(),
                    sizeof(KillEventPacket) * g_killEvents.size());

                if (send(session.socket, sendBuffer, killHeader.size, 0) == SOCKET_ERROR) {
                    LeaveCriticalSection(&g_csSessions);
                    printf("[Send] 클라이언트(%u) KillEvent 전송 실패: %d\n", session.sessionId, WSAGetLastError());
                    DisconnectSession(session);
                    continue;
                }
            }
            LeaveCriticalSection(&g_csSessions);

            const SnapshotPacket& snapshot = g_snapshots;
            PacketHeader header{};
            header.type = SC_SNAPSHOT;
            header.size = sizeof(PacketHeader)
                + sizeof(uint16_t) // playerCount
                + sizeof(PlayerInfo) * snapshot.playerCount // players
                + sizeof(uint16_t) // bulletCount
                + sizeof(Bullet) * snapshot.bulletCount; // bullets

            char* ptr = sendBuffer;
            memcpy(ptr, &header, sizeof(header));
            ptr += sizeof(header);

            memcpy(ptr, &snapshot.playerCount, sizeof(uint16_t));
            ptr += sizeof(uint16_t);
            if (snapshot.playerCount > 0) {
                memcpy(ptr, snapshot.players, sizeof(PlayerInfo) * snapshot.playerCount);
                ptr += sizeof(PlayerInfo) * snapshot.playerCount;
            }

            memcpy(ptr, &snapshot.bulletCount, sizeof(uint16_t));
            ptr += sizeof(uint16_t);
            if (snapshot.bulletCount > 0) {
                memcpy(ptr, snapshot.bullets, sizeof(Bullet) * snapshot.bulletCount);
            }

            if (send(session.socket, sendBuffer, header.size, 0) == SOCKET_ERROR) {
                printf("[Send] 클라이언트(%u) 스냅샷 전송 실패: %d\n", session.sessionId, WSAGetLastError());
                DisconnectSession(session);
            }
            else {

            }
        }
    }

    printf("[NetworkThread] Player %u 종료\n", session.sessionId);
    return 0;
}