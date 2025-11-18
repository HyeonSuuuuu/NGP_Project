#pragma once
#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI NetworkThread(void* args);