#pragma once


// PlayerInfo =====================================
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

// 상수 ===========================================
constexpr int SERVER_PORT = 9000;
constexpr int WORLD_SIZE = 100;
// M_PI를 위한 정의 추가
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
// 에러처리 ========================================
void err_quit(const char* msg);
void err_display(const char* msg);
void err_display(int errCode);