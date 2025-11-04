#include "stdafx.h"
#include "Menu2Scene.h"
#include "GameContext.h"
#include "Mesh.h"

CMenu2Scene::CMenu2Scene(CGameContext& context)
	:CScene(context)
{
	m_rTutorial = { 200, 100, 400, 140 };
	m_rLevel1 = { 200, 150, 400, 190 };
	m_rLevel2 = { 200, 200, 400, 240 };
	m_rStart = { 200, 250, 400, 290 };
	m_rEnd = { 200, 300, 400, 340 };
}

void CMenu2Scene::BuildObjects()
{

}

void CMenu2Scene::ReleaseObjects()
{
}

void CMenu2Scene::Animate(float fElapsedTime)
{
}

void CMenu2Scene::Render(HDC hDCFrameBuffer)
{
	Rectangle(hDCFrameBuffer, m_rTutorial.left, m_rTutorial.top, m_rTutorial.right, m_rTutorial.bottom);
	Rectangle(hDCFrameBuffer, m_rLevel1.left, m_rLevel1.top, m_rLevel1.right, m_rLevel1.bottom);
	Rectangle(hDCFrameBuffer, m_rLevel2.left, m_rLevel2.top, m_rLevel2.right, m_rLevel2.bottom);
	Rectangle(hDCFrameBuffer, m_rStart.left, m_rStart.top, m_rStart.right, m_rStart.bottom);
	Rectangle(hDCFrameBuffer, m_rEnd.left, m_rEnd.top, m_rEnd.right, m_rEnd.bottom);

	DrawCenteredText(hDCFrameBuffer, m_rTutorial, _T("Tutorial"));
	DrawCenteredText(hDCFrameBuffer, m_rLevel1, _T("Level-1"));
	DrawCenteredText(hDCFrameBuffer, m_rLevel2, _T("Level-2"));
	DrawCenteredText(hDCFrameBuffer, m_rStart, _T("Start"));
	DrawCenteredText(hDCFrameBuffer, m_rEnd, _T("End"));

}

void CMenu2Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		POINT ptMouse;
		ptMouse.x = LOWORD(lParam);
		ptMouse.y = HIWORD(lParam);

		if (PtInRect(&m_rTutorial, ptMouse)) {
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::AIRPLANE);
		}
		else if (PtInRect(&m_rLevel1, ptMouse)) {
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::LEVEL1);
		}
		else if (PtInRect(&m_rLevel2, ptMouse)) {
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::LEVEL2);
		}
		else if (PtInRect(&m_rStart, ptMouse)) {
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::LEVEL1);
		}
		else if (PtInRect(&m_rEnd, ptMouse)) {
			::PostQuitMessage(0);
		}
			
		break;
	}
}

void CMenu2Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}


void CMenu2Scene::DrawCenteredText(HDC hDC, const RECT& rect, const TCHAR* text)
{
	SIZE size;
	GetTextExtentPoint32(hDC, text, lstrlen(text), &size);

	int x = (rect.left + rect.right) / 2 - size.cx / 2;
	int y = (rect.top + rect.bottom) / 2 - size.cy / 2;

	Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	TextOut(hDC, x, y, text, lstrlen(text));
}