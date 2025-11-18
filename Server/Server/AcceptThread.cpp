#include "stdafx.h"
#include "Thread.h"

DWORD WINAPI AcceptThread(void* args) {
	SOCKET listen_sock = reinterpret_cast<SOCKET>(args);
	uint32_t idCount = 0;

	while (g_isRunning.load()) {
		// Accept 성공하면
		// Session 생성(동적으로) / vector 추가(Session*)
		// 네트워크 스레드 생성 (인자로 Session 넘겨줌)
	}
	// 모든 세션 connect = False

	return 0;
}