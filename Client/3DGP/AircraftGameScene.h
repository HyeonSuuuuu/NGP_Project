#pragma once
#include "Scene.h"

#define _WITH_DRAW_AXIS

class CAirplanePlayer;
class CGameObject;
class CWallsObject;
class CCamera;

class CAirplaneGameScene : public CScene
{
public:
	CAirplaneGameScene(CGameContext& gameContext);
	virtual ~CAirplaneGameScene();

	void SetPlayer(std::shared_ptr<CAirplanePlayer> player) { m_spPlayer = player; }

private:
	int							m_nObjects = 0;
	CGameObject** m_ppObjects = nullptr;
	std::shared_ptr<CAirplanePlayer>	m_spPlayer = nullptr;
	CGameObject* m_pLockedObject = nullptr;
	CWallsObject* m_pWallsObject = nullptr;
	POINT						m_ptOldCursorPos;

#ifdef _WITH_DRAW_AXIS
	CGameObject* m_pWorldAxis = NULL;
#endif

public:
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer) override;

	virtual void ProcessInput() override;
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, std::shared_ptr<CCamera> pCamera);
};