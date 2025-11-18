#pragma once



void InitNetwork(SOCKET socket);
void WaitAllRecvEvent(std::array<HANDLE, MAX_PLAYER>& arr);