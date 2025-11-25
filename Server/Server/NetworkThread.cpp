#include "stdafx.h"
#include "Thread.h"
#include "Session.h"

DWORD WINAPI NetworkThread(void* args)
{
	// recv, send 버퍼 필요함	
	Session& session = *reinterpret_cast<Session*>(args);

	// Send (SC_ENTER)
	EnterPacket enterPacket;
	enterPacket.id = session.sessionId;

	while (true /* Session의 Connected로 변경 */ ) {
	// Recv (INPUT_PACKET밖에 없음)
	// recv_event Set
	// send_event 기다림
	// send (킬패킷 vector가 empty면 무시, 아니면 모두 send, 스냅샷 send
	}	
	
	return 0;
}


