#pragma once
#include "Scene.h"

class CExplosiveObject;
class CLevel1Player;

class CMenuScene : public CScene
{
public:
	CMenuScene(CGameContext& context);

public:
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer) override;

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	void SetPlayer(std::shared_ptr<CLevel1Player> spPlayer) { m_spPlayer = spPlayer; }

private:
	LONG							m_fAngle = 0;
	RECT							m_rName;
	bool							m_bStart = false;
	std::shared_ptr<CExplosiveObject> m_explosiveObj = nullptr;
	std::shared_ptr<CLevel1Player>	m_spPlayer = nullptr;
};