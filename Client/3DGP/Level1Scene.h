#pragma once
#include "Scene.h"

class CLevel1Player;

class CGameObject;

class CLevel1Scene : public CScene
{
public:
	CLevel1Scene(CGameContext& gameContext);
	virtual ~CLevel1Scene();
	void SetPlayer(std::shared_ptr<CLevel1Player> player) { m_spPlayer = player; }

private:
	std::shared_ptr<CLevel1Player>	m_spPlayer = nullptr;
	std::vector<CGameObject>		m_objects;
	POINT							m_ptOldCursorPos;
	float							m_t = 0;

public:
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer) override;
	
	void GenerateRollerCoaster();

	virtual void ProcessInput() override;
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
};