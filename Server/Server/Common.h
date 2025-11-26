#pragma once


// PlayerInfo =====================================
#pragma pack(push, 1)
struct PlayerInfo {
	uint32_t id;
	float x, z;
	float yawAngle;
	uint16_t hp;
	int32_t gold;
	bool isDead;
	uint16_t killCount;
	uint16_t deathCount;
};

// Obstacle =======================================
struct Obstacle {
	float x, z;
	float size;
};

// Bullet =========================================
struct Bullet
{
	float x, z;
	float yawAngle; // 0 ~ 360
	float speed;
	int ownerId;
};
#pragma pack(pop)

// 상수 ===========================================
constexpr int SERVER_PORT = 9000;
constexpr int WORLD_SIZE = 100;
// M_PI를 위한 정의 추가
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
constexpr int MAX_PLAYERS = 64; // 이벤트 최대가 64

#define INP_FORWARD				0x01
#define INP_BACKWARD			0x02
#define INP_LEFT				0x04
#define INP_RIGHT				0x08
#define INP_ONE					0x10
#define INP_TWO					0x20
#define INP_THREE				0x40
#define INP_SPACEBAR			0x80

// Log 함수
void DebugLog(const wchar_t* fmt, ...);

// 에러처리 ========================================
void err_quit(const char* msg);
void err_display(const char* msg);
void err_display(int errCode);