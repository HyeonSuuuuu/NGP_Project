//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	::srand(timeGetTime());
	m_gameContext.m_hInstance = hInstance;
	m_gameContext.m_hWnd = hMainWnd;

	m_gameContext.m_sceneManager.Init(m_gameContext);

	BuildFrameBuffer(); 

	BuildObjects(); 

	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_gameContext.m_hBitmapFrameBuffer) ::DeleteObject(m_gameContext.m_hBitmapFrameBuffer);
	if (m_gameContext.m_hDCFrameBuffer) ::DeleteDC(m_gameContext.m_hDCFrameBuffer);
}

void CGameFramework::BuildFrameBuffer()
{
	::GetClientRect(m_gameContext.m_hWnd, &m_gameContext.m_rcClient);
	
	HDC hDC = ::GetDC(m_gameContext.m_hWnd);

    m_gameContext.m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_gameContext.m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC,m_gameContext.m_rcClient.right - m_gameContext.m_rcClient.left, m_gameContext.m_rcClient.bottom - m_gameContext.m_rcClient.top);
    ::SelectObject(m_gameContext.m_hDCFrameBuffer, m_gameContext.m_hBitmapFrameBuffer);

	::ReleaseDC(m_gameContext.m_hWnd, hDC);
    ::SetBkMode(m_gameContext.m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_gameContext.m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_gameContext.m_hDCFrameBuffer, hBrush);
	::Rectangle(m_gameContext.m_hDCFrameBuffer, m_gameContext.m_rcClient.left, m_gameContext.m_rcClient.top, m_gameContext.m_rcClient.right, m_gameContext.m_rcClient.bottom);
	::SelectObject(m_gameContext.m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_gameContext.m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_gameContext.m_hWnd);
	const RECT& rcClient = m_gameContext.m_rcClient;
    ::BitBlt(hDC, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, m_gameContext.m_hDCFrameBuffer, rcClient.left, rcClient.top, SRCCOPY);
    ::ReleaseDC(m_gameContext.m_hWnd, hDC);
}

void CGameFramework::BuildObjects()
{
}

void CGameFramework::ReleaseObjects()
{
	const auto& curScene = m_gameContext.m_sceneManager.GetCurScene();
	if (curScene)
	{
		curScene->ReleaseObjects();
	}

}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	const auto& curScene = m_gameContext.m_sceneManager.GetCurScene();
	if (curScene) curScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	const auto& curScene = m_gameContext.m_sceneManager.GetCurScene();
	if (curScene) curScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_gameContext.m_gameTimer.Stop();
		else
			m_gameContext.m_gameTimer.Start();
		break;
	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	const auto& curScene = m_gameContext.m_sceneManager.GetCurScene();
	if (curScene) curScene->ProcessInput();
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_gameContext.m_gameTimer.GetTimeElapsed();
	const auto& curScene = m_gameContext.m_sceneManager.GetCurScene();
	if (curScene) curScene->Animate(fTimeElapsed);
}

void CGameFramework::FrameAdvance()
{    
	m_gameContext.m_gameTimer.Tick(60.0f);

	ProcessInput();

	AnimateObjects();

    ClearFrameBuffer(RGB(255, 255, 255));

	// 카메라를 어디다 두어야 할까
	Render();

	PresentFrameBuffer();

	m_gameContext.m_gameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_gameContext.m_hWnd, m_pszFrameRate);
}

void CGameFramework::Render()
{
	const auto& curScene = m_gameContext.m_sceneManager.GetCurScene();
	if (curScene) curScene->Render(m_gameContext.m_hDCFrameBuffer);
}


