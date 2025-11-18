#pragma once



void InitNetwork(SOCKET socket);
inline float RandF(float fMin, float fMax);
void WaitAllRecvEvent(std::array<HANDLE, MAX_PLAYER>& arr);