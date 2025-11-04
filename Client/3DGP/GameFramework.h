#pragma once

#include "Player.h"
#include "GameContext.h"
#include "SceneManager.h"
#include "Timer.h"



class CGameFramework
{
public:
	CGameFramework() { }
	~CGameFramework() { }

private:


	bool						m_bActive = true;

	CGameContext				m_gameContext;
    HBITMAP						m_hBitmapSelect = nullptr;

	//CPlayer*					m_pPlayer = nullptr;
	CGameObject*				m_pLockedObject = nullptr;


	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[50];

public:
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void Render();
	void AnimateObjects();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetActive(bool bActive) { m_bActive = bActive; }
};

