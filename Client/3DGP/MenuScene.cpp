#include "stdafx.h"
#include "MenuScene.h"
#include "GameContext.h"
#include "GameObject.h"
#include "Player.h"
#include "GraphicsPipeline.h"


void CMenuScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();
	CCubeMesh* pCubeMesh = new CCubeMesh(4.f, 4.f, 4.f);
	m_explosiveObj = std::make_shared<CExplosiveObject>();
	m_explosiveObj->SetMesh(pCubeMesh);
	m_explosiveObj->SetColor(RGB(255, 0, 0));
	m_explosiveObj->SetPosition(0.f, -3.f, 0.f);
	m_explosiveObj->m_bBlowingUp = false;
}


CMenuScene::CMenuScene(CGameContext& context)
	: CScene(context)
{
}

void CMenuScene::ReleaseObjects()
{
}

void CMenuScene::Animate(float fElapsedTime)
{
	if (m_bStart && m_explosiveObj->m_bBlowingUp == false)
	m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::MENU2);

	m_explosiveObj->Animate(fElapsedTime);
}

void CMenuScene::Render(HDC hDCFrameBuffer)
{
	std::shared_ptr<CCamera> spCamera = m_spPlayer->GetCamera();
	CGraphicsPipeline::SetViewport(&spCamera->m_Viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&spCamera->m_xmf4x4ViewPerspectiveProject);

	if (m_bStart) {
		m_explosiveObj->Render(hDCFrameBuffer, m_spPlayer->GetCamera());
	}
	const RECT& rcClient = m_gameContext.m_rcClient;
	int centerX = (rcClient.left + rcClient.right) / 2;
	int centerY = (rcClient.top + rcClient.bottom) / 2;


	LOGFONT nameFont = { 0 };
	nameFont.lfWeight = FW_BOLD;
	nameFont.lfEscapement = m_fAngle++ * 50;
	LOGFONT titleFont = { 0 };
	titleFont.lfWeight = FW_BOLD;

	HFONT hFont = CreateFontIndirect(&titleFont);
	HFONT hRotateFont = CreateFontIndirect(&nameFont);
	HFONT hOldFont = (HFONT)SelectObject(hDCFrameBuffer, hFont);


	SetBkMode(hDCFrameBuffer, TRANSPARENT);
	SetTextColor(hDCFrameBuffer, RGB(0, 0, 0));


	RECT rTitle = { centerX - 200, centerY - 100 - 100, centerX + 200, centerY + 100 - 100 };
	m_rName = { centerX - 90 , centerY - 100 + 100, centerX + 50, centerY + 50 + 100 };

	const char* pszTitle = "3D 게임프로그래밍 1";
	const char* pszName = "이현수";
	DrawTextA(hDCFrameBuffer, pszTitle, -1, &rTitle, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(hDCFrameBuffer, hRotateFont);

	if (m_bStart == false) {
		DrawTextA(hDCFrameBuffer, pszName, -1, &m_rName, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
	SelectObject(hDCFrameBuffer, hOldFont);
	DeleteObject(hRotateFont);
	DeleteObject(hFont);
}

void CMenuScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		POINT ptMouse;
		ptMouse.x = LOWORD(lParam);
		ptMouse.y = HIWORD(lParam);

		if (PtInRect(&m_rName, ptMouse))
		{
			m_bStart = true;
			m_explosiveObj->m_bBlowingUp = true;
		}
		break;
	}
}

void CMenuScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}