#include "stdafx.h"
#include "Thread.h"
#include "Session.h"
#include "Packet.h"
#include "Globals.h"
#include <iostream>
#include <vector>

int count;

DWORD WINAPI NetworkThread(void* args)
{
    int retval;
    Session& session = *reinterpret_cast<Session*>(args);
    
    // buffer 하나로 recv/send
    char buffer[65536];

    SendEnterPacket(buffer, session);

    // 전역 세션 리스트에 추가 (스레드 세이프)
    EnterCriticalSection(&g_csSessions);
    g_sessions.push_back(&session);
    std::cout << "현재 서버 인원: " << g_sessions.size() << "명" << std::endl;
    LeaveCriticalSection(&g_csSessions);

    PacketHeader header;
    while (session.isConnected.load()) {
        count++;
        retval = recv(session.socket, (char*)&header, sizeof(PacketHeader), MSG_WAITALL);
        if (retval != sizeof(PacketHeader))
            break;
        if (header.type == CS_INPUT) {
            InputPacket inputPkt;
            if (sizeof(InputPacket) != header.size)
                break;
            retval = recv(session.socket, (char*)&inputPkt, header.size, MSG_WAITALL);
            if (retval != header.size)
                break;
            session.inputflag = inputPkt.inputFlag;
            session.data.yawAngle = static_cast<float>(inputPkt.yawAngle);
            if (!(count % 30))
                DebugLog("[패킷 Recv] CS_INPUT (Angle: %hd, inputFlag: %d\n", inputPkt.yawAngle, inputPkt.inputFlag);
        }

        // recv_event Set
        SetEvent(session.recvEvent);
        // send_event 기다림
       
        // send
        SendSnapshotPacket(buffer, session);
    }

    DisconnectSession(session);
    return 0;
}


void SendEnterPacket(char* buffer, Session& session)
{
    PacketHeader header{};
    header.type = SC_ENTER;
    uint32_t offset = 0;

    EnterPacket enterPkt{};
    enterPkt.id = session.sessionId;
    enterPkt.obstacleCount = static_cast<uint16_t>(g_obstacles.size());

    offset += sizeof(PacketHeader);
    memcpy(buffer + offset, &enterPkt, sizeof(EnterPacket));
    offset += sizeof(EnterPacket);
    memcpy(buffer + offset, g_obstacles.data(), enterPkt.obstacleCount * sizeof(Obstacle));
    offset += enterPkt.obstacleCount * sizeof(Obstacle);
    header.size = offset - sizeof(PacketHeader);
    memcpy(buffer, &header, sizeof(PacketHeader));
    send(session.socket, buffer, offset, 0);
    DebugLog("[패킷 전송] SC_ENTER -> Player %u (장애물 %llu개 동기화)\n", session.sessionId, g_obstacles.size());
}

// RecvPacket
void ProcessPacket(Session& session, const PacketHeader packet)
{
    
}

void SendSnapshotPacket(char* buffer, Session& session)
{
    PacketHeader header{};
    header.type = SC_SNAPSHOT;
    uint32_t offset = 0;

    SnapshotPacket pkt{};
    pkt.playerCount = g_players.size();
    pkt.bulletCount = g_bullets.size();

    offset += sizeof(PacketHeader);
    memcpy(buffer + offset, &pkt, sizeof(SnapshotPacket));
    offset += sizeof(SnapshotPacket);

    memcpy(buffer + offset, g_players.data(), pkt.playerCount * sizeof(PlayerInfo));
    offset += pkt.playerCount * sizeof(PlayerInfo);

    memcpy(buffer + offset, g_bullets.data(), pkt.bulletCount * sizeof(Bullet));
    offset += pkt.bulletCount * sizeof(Bullet);

    header.size = offset - sizeof(PacketHeader);
    memcpy(buffer, &header, sizeof(PacketHeader));
    send(session.socket, buffer, offset, 0);
    if (!(count % 30))
        DebugLog("[패킷 전송] SC_SNAPSHOT -> Player ()\n", session.sessionId);
}


// 클라이언트 연결 해제 처리
void DisconnectSession(Session& session)
{
    EnterCriticalSection(&g_csSessions);
    auto it = std::find_if(g_sessions.begin(), g_sessions.end(), 
        [&session](const Session* const s) { return s->socket == session.socket; });

    if (it != g_sessions.end())
        g_sessions.erase(it);
    LeaveCriticalSection(&g_csSessions);

    closesocket(session.socket);

    printf("[연결 해제] Player %u 연결 종료\n", session.sessionId);
}