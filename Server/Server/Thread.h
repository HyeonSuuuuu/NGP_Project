#pragma once

struct Session;

DWORD WINAPI AcceptThread(void* args);
DWORD WINAPI NetworkThread(void* args);



void SendEnterPacket(char* buffer, Session& session);
void ProcessPacket(Session& session, const PacketHeader header);
void SendSnapshotPacket(char* buffer, Session& session);
void DisconnectSession(Session& session);