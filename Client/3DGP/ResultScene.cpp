#include "stdafx.h"
#include "ResultScene.h"
#include "GameContext.h"
#include "GameObject.h"
#include "Player.h"
#include "GraphicsPipeline.h"


void CResultScene::BuildObjects()
{

}


CResultScene::CResultScene(CGameContext& context)
	: CScene(context)
{
	m_rLabel = { 200, 200, 400, 240 };
}

void CResultScene::ReleaseObjects()
{
}

void CResultScene::Animate(float fElapsedTime)
{
}

void CResultScene::Render(HDC hDCFrameBuffer)
{

	if (m_gameContext.m_bWin)
		DrawCenteredText(hDCFrameBuffer, m_rLabel, _T("You Win"));
	else
		DrawCenteredText(hDCFrameBuffer, m_rLabel, _T("You Lose"));

}

void CResultScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CResultScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::MENU2);
			break;
		case VK_RETURN:
			break;
		case VK_CONTROL:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CResultScene::DrawCenteredText(HDC hDC, const RECT& rect, const TCHAR* text)
{
	SIZE size;
	GetTextExtentPoint32(hDC, text, lstrlen(text), &size);

	int x = (rect.left + rect.right) / 2 - size.cx / 2;
	int y = (rect.top + rect.bottom) / 2 - size.cy / 2;

	Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	TextOut(hDC, x, y, text, lstrlen(text));
}