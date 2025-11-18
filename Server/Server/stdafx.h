#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// C
#include <tchar.h>

// C++

#include <iostream>
#include <atomic>
#include <vector>
#include <array>
#include <mutex>
#include <thread>

// User Defined
#include "Packet.h"
#include "Common.h"
#include "Thread.h"
#include "Globals.h"