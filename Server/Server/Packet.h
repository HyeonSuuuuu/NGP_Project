#pragma once

#pragma pack(push, 1)
struct PacketHeader {
	uint16_t type;
	uint16_t size;
};
#pragma pack(pop)

enum PacketType : uint16_t {
	CS_INPUT		= 101,

	SC_ENTER		= 201,
	SC_SNAPSHOT		= 202,
	SC_KILL_EVENT	= 203,
};
#pragma pack(push, 1)
struct InputPacket {
	uint32_t id;
	uint16_t inputFlag;
	int16_t yawAngle;
};
struct EnterPacket{
	  uint32_t id;
	  struct Obstacle* obstacles;
	  uint16_t obstacle_count;
}
struct SnapshotPacket {
	uint16_t player_count;
	struct PlayerInfo* players;
	uint16_t bullet_count;
	struct Bullet* bullets;
};

struct KillEventPacket {
	int32_t killer_id;
	int32_t killed_id;
};
#pragma pack(pop)