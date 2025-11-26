#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

void InitNetwork(SOCKET* listen_socket);
void WaitAllRecvEvent(std::array<HANDLE, MAX_PLAYER>& arr);