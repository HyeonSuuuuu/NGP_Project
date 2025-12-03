#include "stdafx.h"
#include "Level1Scene.h"
#include "GameContext.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"
#include "Player.h"

CLevel1Scene::CLevel1Scene(CGameContext& gameContext)
	: CScene(gameContext)
{

}
CLevel1Scene::~CLevel1Scene()
{
}

void CLevel1Scene::BuildObjects()
{
	GenerateRollerCoaster();

}

void CLevel1Scene::ReleaseObjects()
{
}

void CLevel1Scene::Animate(float fElapsedTime)
{
	if (m_spPlayer->isPathEnd()) {
		m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::LEVEL2);
	}
	for (auto& obj : m_objects) {
		obj.Animate(fElapsedTime);
	}
	m_spPlayer->Animate(fElapsedTime);
}

void CLevel1Scene::Render(HDC hDCFrameBuffer)
{
	std::shared_ptr<CCamera> spCamera = m_spPlayer->GetCamera();
	CGraphicsPipeline::SetViewport(&spCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&spCamera->m_xmf4x4ViewPerspectiveProject);

	for (auto& obj : m_objects) {
		obj.Render(hDCFrameBuffer, spCamera);
	}
	m_spPlayer->Render(hDCFrameBuffer, spCamera);
}

void CLevel1Scene::GenerateRollerCoaster()
{
	float meshLength = 3.0f; // 큐브 하나의 길이
	CCubeMesh* pMesh = new CCubeMesh(1.0f, 1.0f, meshLength); // Width, Height, Depth

	m_spPlayer->AddPoint(XMFLOAT3(0, 0, 0));
	m_spPlayer->AddPoint(XMFLOAT3(5, 5, 30));
	m_spPlayer->AddPoint(XMFLOAT3(10, 10, 70));
	m_spPlayer->AddPoint(XMFLOAT3(30, 15, 110)); // 최고점
	m_spPlayer->AddPoint(XMFLOAT3(50, 5, 150));
	m_spPlayer->AddPoint(XMFLOAT3(80, -10, 200)); // 급하강
	m_spPlayer->AddPoint(XMFLOAT3(110, 0, 250));
	m_spPlayer->AddPoint(XMFLOAT3(140, 5, 300));
	m_spPlayer->AddPoint(XMFLOAT3(160, 15, 320)); // 다시 상승
	m_spPlayer->AddPoint(XMFLOAT3(180, 10, 350));
	m_spPlayer->AddPoint(XMFLOAT3(200, 0, 380));
	m_spPlayer->AddPoint(XMFLOAT3(230, -10, 410));
	m_spPlayer->AddPoint(XMFLOAT3(260, -5, 440));
	m_spPlayer->AddPoint(XMFLOAT3(300, 0, 500)); // 마지막 완만한 하강

	float estimatedPathLength = 500.0f;
	int objCount = static_cast<int>(estimatedPathLength / meshLength); // 약 166개
	m_objects.resize(objCount);

	for (int i = 0; i < objCount; ++i)
	{
		float t = static_cast<float>(i) / objCount;

		m_objects[i].SetMesh(pMesh);

		// 위치 및 방향 계산
		XMFLOAT3 position = m_spPlayer->GetPosition(t);
		m_objects[i].SetPosition(position);
		XMFLOAT3 tangent = m_spPlayer->GetTangent(t);
		// 플레이어 방향
		m_spPlayer->LookAt(tangent,XMFLOAT3(0.f, 1.f, 0.f));

		// 업 벡터에 약간의 트위스트를 줘서 회전 느낌 추가
		float angle = XM_PI * 2.0f * t; // 0 ~ 2pi 회전
		XMFLOAT3 up = Vector3::Normalize(XMFLOAT3(sinf(angle) * 0.5f, 1.0f, cosf(angle) * 0.5f));

		m_objects[i].LookTo(tangent, up);
	}

}

void CLevel1Scene::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	//if (GetKeyboardState(pKeyBuffer))
	//{
	//	DWORD dwDirection = 0;
	//	if (pKeyBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	//	if (pKeyBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	//	if (pKeyBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	//	if (pKeyBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

	//	if (dwDirection)
	//		m_spPlayer->Move(dwDirection, 0.15f);
	//}

	//if (GetCapture() == m_gameContext.m_hWnd)
	//{
	//	SetCursor(NULL);
	//	POINT ptCursorPos;
	//	GetCursorPos(&ptCursorPos);
	//	float cxMouseDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x);
	//	float cyMouseDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y);
	//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//	if (cxMouseDelta)
	//	{
	//		m_spPlayer->Rotate(0.f, cxMouseDelta, 0.f);
	//	}
	//}

	m_spPlayer->Update(m_gameContext.m_gameTimer.GetTimeElapsed());
}

void CLevel1Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CLevel1Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::ReleaseCapture();
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::MENU2);
			break;
		case VK_RETURN:
			break;
		case 'N':
			::ReleaseCapture();
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::LEVEL2);
		default:
			break;
		}
		break;
	default:
		break;
	}
}

