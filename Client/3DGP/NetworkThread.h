#pragma once


DWORD WINAPI NetworkThread(void* args);

void ConnectServer(SOCKET& sock);
void ProcessEnterPacket(char* buf);