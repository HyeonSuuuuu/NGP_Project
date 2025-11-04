#pragma once
#include "Scene.h"

class CExplosiveObject;
class CLevel1Player;

class CResultScene : public CScene
{
public:
	CResultScene(CGameContext& context);

public:
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer) override;


	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	void DrawCenteredText(HDC hDC, const RECT& rect, const TCHAR* text);
private:
	RECT							m_rLabel;
	bool							m_bWin = false;
};

