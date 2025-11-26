#include "stdafx.h"
#include "Thread.h"
#include "Session.h"
#include "Packet.h"
#include "Globals.h"
#include <iostream>

// 패킷 수신 처리 함수
void ProcessPacket(Session& session, const PacketHeader* header)
{
    if (header->type == CS_INPUT) {
        const InputPacket* inputPkt = reinterpret_cast<const InputPacket*>(header + 1);

        // 입력 정보(inputFlag, yawAngle)를 해당 세션에 저장
        session.inputflag = inputPkt->inputFlag;
        session.data.yawAngle = static_cast<float>(inputPkt->yawAngle);
    }
}

// 클라이언트 연결 해제 처리
void DisconnectSession(Session& session)
{
    // WSAEventSelect 해제 및 소켓 닫기
    WSAEventSelect(session.socket, NULL, 0);
    closesocket(session.socket);

    // 세션 연결 상태 업데이트
    session.isConnected.store(false);

    // 메인 스레드가 g_sessions에서 제거할 수 있도록 플래그만 설정
    printf("[연결 해제] Player %u 연결 종료\n", session.sessionId);
}

// -1: 에러, 0: 연결 종료, 1: 성공
int HandleRecv(Session& session, char* buffer, const int bufferSize)
{
    // 패킷 헤더만 먼저 받기
    int nRecv = recv(session.socket, buffer, sizeof(PacketHeader), MSG_PEEK);

    if (nRecv == 0) return 0; // 연결 종료
    if (nRecv == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) return 1; // 아직 데이터 없음
        return -1; // 에러
    }

    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);

    if (header->size > bufferSize) {
        // 패킷 크기가 버퍼보다 크면 문제 발생
        printf("[ERROR] 패킷 크기 초과: %u\n", header->size);
        return -1;
    }

    // 패킷 전체 받기 (블로킹으로 처리한다고 가정)
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
    constexpr int MAX_BUFFER_SIZE = 1024 * 4; // 4KB (임의 지정)
    char recvBuffer[MAX_BUFFER_SIZE]{};
    char sendBuffer[MAX_BUFFER_SIZE]{};

    // Send 이벤트를 기다릴 핸들 목록
    HANDLE waitHandles[2] = { session.recvEvent, g_sendevent };

    while (session.isConnected.load()) {

        // 1. 이벤트 대기 (Recv 또는 Send 신호)
        DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {
            // 2. Recv Event (session.recvEvent) 발생: 데이터 수신 처리

            WSANETWORKEVENTS netEvents;
            WSAEnumNetworkEvents(session.socket, session.recvEvent, &netEvents);

            // FD_READ (데이터 도착) 처리
            if (netEvents.lNetworkEvents & FD_READ) {
                int result = HandleRecv(session, recvBuffer, MAX_BUFFER_SIZE);
                if (result <= 0) { // 연결 종료 또는 에러
                    if (result == 0) printf("[Recv] 클라이언트(%u) 연결 종료 요청 감지\n", session.sessionId);
                    else printf("[Recv] 클라이언트(%u) 수신 에러: %d\n", session.sessionId, netEvents.iErrorCode[FD_READ_BIT]);
                    DisconnectSession(session);
                    continue;
                }
            }
            // FD_CLOSE (소켓 종료) 처리
            if (netEvents.lNetworkEvents & FD_CLOSE) {
                printf("[Close] 클라이언트(%u) 연결 끊김 감지: %d\n", session.sessionId, netEvents.iErrorCode[FD_CLOSE_BIT]);
                DisconnectSession(session);
                continue;
            }

        }
        else if (waitResult == (WAIT_OBJECT_0 + 1)) {

            // 스냅샷 패킷 전송
            const SnapshotPacket& snapshot = g_snapshots;
            PacketHeader header{};
            header.type = SC_SNAPSHOT;
            header.size = sizeof(PacketHeader)
                + sizeof(uint16_t) // playerCount
                + sizeof(PlayerInfo) * snapshot.playerCount // players 배열
                + sizeof(uint16_t) // bulletCount
                + sizeof(Bullet) * snapshot.bulletCount; // bullets 배열

            // 임시 버퍼에 패킷 조립 및 전송
            char* ptr = sendBuffer;
            memcpy(ptr, &header, sizeof(header));
            ptr += sizeof(header);

            // playerCount와 players 복사
            memcpy(ptr, &snapshot.playerCount, sizeof(uint16_t));
            ptr += sizeof(uint16_t);
            if (snapshot.playerCount > 0) {
                memcpy(ptr, snapshot.players, sizeof(PlayerInfo) * snapshot.playerCount);
                ptr += sizeof(PlayerInfo) * snapshot.playerCount;
            }

            // bulletCount와 bullets 복사
            memcpy(ptr, &snapshot.bulletCount, sizeof(uint16_t));
            ptr += sizeof(uint16_t);
            if (snapshot.bulletCount > 0) {
                memcpy(ptr, snapshot.bullets, sizeof(Bullet) * snapshot.bulletCount);
            }

            // 실제 전송
            if (send(session.socket, sendBuffer, header.size, 0) == SOCKET_ERROR) {
                printf("[Send] 클라이언트(%u) 스냅샷 전송 실패: %d\n", session.sessionId, WSAGetLastError());
                DisconnectSession(session);
            }
            else {

            }
        }
    }

    // 세션 연결 해제 시 처리 (NetworkThread 종료)

    printf("[NetworkThread] Player %u 종료\n", session.sessionId);
    return 0;
}