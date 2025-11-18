#pragma once


// PlayerInfo =====================================
struct PlayerInfo {
	uint32_t id;
	float x, y;
	float yawAngle;
	uint16_t hp;
	int32_t gold;
	bool isDead;
	uint16_t killCount;
	uint16_t deathCount;
};

// Obstacle =======================================
struct Obstacle {
	uint32_t entityId;
	float x, y;
	float size;
};

// Bullet =========================================
struct Bullet
{
	float x, y;
	float yawAngle; // 0 ~ 360
	float speed;
	int ownerId;
};

// 상수 ===========================================
#define SERVER_PORT			9000
#define WORLD_SIZE			100
// 에러처리 ========================================
void err_quit(const char* msg);
void err_display(const char* msg);
void err_display(int errCode);