class PlayerInfo;
class Obstacle;

struct PacketHeader {
	uint16_t type;
	uint16_t size;
};

enum PacketType : uint16_t {
	CS_INPUT = 101,
	CS_BUY_HP = 102,
	CS_BUY_ATK = 103,
	CS_BUY_HEAL = 104,

	SC_SNAPSHOT = 201,
	SC_ENTITY_SPAWN = 202,
	SC_KILL_EVENT = 203,
	SC_DEAD_EVENT = 204,
	SC_RESPONSE_EVENT = 205,
};

struct InputPacket {
	uint32_t id;
	uint32_t tick;
	uint8_t input;
	int16_t yawAngle;
};

struct SnapshotPacket {
	uint32_t tick;
	uint16_t count;
	PlayerInfo* players;
};

struct EntitySpawnPacket {
	uint16_t count;
	Obstacle* obstacles;
};

struct KillEventPacket {
	int32_t killer_id;
	int32_t killed_id;
};

struct DeadEventPacket {
	int32_t tick;
};
struct ResponeEventPacket {
	float x, y;
};
