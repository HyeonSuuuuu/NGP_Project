#pragma once
#include "Scene.h"
#include "../../Server/Server/Common.h"
#include "GameObject.h"

class CGameObject;
class CLevel2Player;
class CEnemyObject;
class CBarrierObject;
class CCamera;

class CLevel2Scene : public CScene
{
public:
	CLevel2Scene(CGameContext& gameContext);
	virtual ~CLevel2Scene();

public:
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer) override;

	virtual void ProcessInput() override;
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual void Enter();
	virtual void Exit();


	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();
	void CheckPlayerByEnemyCollisions();
	void SetPlayer(std::shared_ptr<CLevel2Player> player) { m_spPlayer = player; }
	CEnemyObject* PickObjectPointedByCursor(int xClient, int yClient, std::shared_ptr<CCamera> spCamera);

private:
	std::shared_ptr<CLevel2Player>						m_spPlayer = nullptr;
	POINT												m_ptOldCursorPos;
	std::vector<CGameObject>							m_objects;
	std::array<CEnemyObject, MAX_PLAYERS-1>				m_enemyObjects;
	std::array<CBulletObject, 300>						m_bullets;
	int													m_nBullets = 0;
	std::vector<CBarrierObject>							m_barrierObjects;
	CEnemyObject*										m_pLockedObject = nullptr;
	bool												m_bAutoAttack = false;
	bool												m_bShield = false;
};