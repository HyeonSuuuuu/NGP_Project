#pragma once
#include "Player.h"

class GameManager {
public:
	GameManager();
	~GameManager() = default;
	
	void Update(float deltaTime, const std::vector<bool>& keyStates);
	void Render(HWND hWnd);

	void Init(HWND hWnd);
private:
	// 게임 상태 변수
	Player m_player;
	std::vector<Feed> m_feeds;
	
	// 
	float WINDOW_HALF_WIDTH;
	float WINDOW_HALF_HEIGHT;

	// 난수 생성기
	std::random_device m_rd;
	std::mt19937 m_gen;
	std::uniform_real_distribution<float> m_positionDis;
	std::uniform_int_distribution<int> m_colorDis;

	// helper function
	bool checkCollision(const POSITION& playerPosition, float playerSize, const POSITION& otherPosition, float otherSize);
	void redrawAgarioLine(HDC hdc, const POSITION& playerPosition, float width, float height, float cameraZoom);
};