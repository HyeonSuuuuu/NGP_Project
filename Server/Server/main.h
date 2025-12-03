#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

// Input Flags
constexpr uint16_t INPUT_MOVE_FORWARD = 0x01;
constexpr uint16_t INPUT_MOVE_BACKWARD = 0x02;
constexpr uint16_t INPUT_MOVE_LEFT = 0x04;
constexpr uint16_t INPUT_MOVE_RIGHT = 0x08;
constexpr uint16_t INPUT_FIRE = 0x10;


void InitNetwork(SOCKET* listen_socket);
void WaitAllRecvEvent(std::array<HANDLE, MAX_PLAYER>& arr);