#include "Timer.h"
#include "Session.h"




// 전역변수 =======================================
extern GameTimer g_timer;
extern std::vector<Session> g_sessions;
extern CRITICAL_SECTION g_csSessions;
extern std::vector<Obstacle> g_obstacles;
extern std::atomic<bool> isRunning;

void InitGlobals();

// 상수 ===========================================
#define SERVER_PORT 9000


// 에러처리 ========================================
void err_quit(const char* msg);
void err_display(const char* msg);
void err_display(int errCode);