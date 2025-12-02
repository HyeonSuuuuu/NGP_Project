#include "stdafx.h"
#include "Level2Scene.h"
#include "Player.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"
#include "GameContext.h"
#include "Globals.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

CLevel2Scene::CLevel2Scene(CGameContext& gameContext)
	: CScene(gameContext)
{
}

CLevel2Scene::~CLevel2Scene()
{
}

void CLevel2Scene::BuildObjects()
{
	m_objects.resize(1);

	CGroundMesh* pGroundMesh = new CGroundMesh(100, 0, 100, 10);
	m_objects[0].SetMesh(pGroundMesh);
	m_objects[0].SetColor(RGB(0, 0, 0));
	m_objects[0].SetPosition(0.f, 0.f, 40.f);
	
	WaitForSingleObject(g_enterEvent, INFINITE);
	CCubeMesh* pCubeMesh = new CCubeMesh(4.f, 4.f, 4.f);
	m_barrierObjects.reserve(g_obstacles.size());
	for (const Obstacle& obstacle : g_obstacles) {
		m_barrierObjects.emplace_back(pCubeMesh, obstacle.x, 0.f, obstacle.z);
	}

	CTankMesh* pTankMesh = new CTankMesh(6.f, 3.f, 6.f);
	for (int i = 0; i < 63; ++i) {
		m_enemyObjects[i].SetMesh(pTankMesh);
		m_enemyObjects[i].SetColor(RGB(255, 0, 0));
	}

	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 300; ++i) {
		m_bullets[i].SetMesh(pBulletMesh);
		m_bullets[i].SetColor(RGB(255, 0, 0));
		m_bullets[i].SetRotationAxis(XMFLOAT3(0.f, 0.f, 1.f));
		m_bullets[i].SetRotationSpeed(360.0f);
		m_bullets[i].SetActive(true);
	}
	
}

void CLevel2Scene::ReleaseObjects()
{
}

void CLevel2Scene::Animate(float fElapsedTime)
{
	// Globals.h 이용해서 Update
	EnterCriticalSection(&g_csPlayers);
	g_enemyCount = g_players.size()-1;

	int j = 0;
	for (int i = 0; i < g_enemyCount+1; ++i) {
		if (g_players[i].id == g_myId) {
			m_spPlayer->isDead = g_players[i].isDead;
			if (m_spPlayer->isDead)
				continue;
			m_spPlayer->SetPosition(g_players[i].x, 0, g_players[i].z);
			m_spPlayer->hp = g_players[i].hp;
			m_spPlayer->gold = g_players[i].gold;
			m_spPlayer->maxHp = g_players[i].maxHp;
			m_spPlayer->atk = g_players[i].atk;
			m_spPlayer->killCount = g_players[i].killCount;
			m_spPlayer->deathCount = g_players[i].deathCount;
		}
		else {
			m_enemyObjects[j].SetPosition(g_players[i].x, 0, g_players[i].z);
			m_enemyObjects[j].SetYawRotation(g_players[i].yawAngle);
			m_enemyObjects[j].m_isDead = g_players[i].isDead;
			j++;
		}
	}
	LeaveCriticalSection(&g_csPlayers);

	EnterCriticalSection(&g_csBullets);
/*	if (m_nBullets > g_bullets.size()) {
		for (int i = g_bullets.size(); i < m_nBullets; ++i)
			m_bullets[i].SetActive(false);
	}*/
	m_nBullets = g_bullets.size();

	for (int i = 0; i < m_nBullets; ++i) {
		m_bullets[i].SetPosition(g_bullets[i].x, 0.f, g_bullets[i].z);
	}
	LeaveCriticalSection(&g_csBullets);

	m_spPlayer->Animate(fElapsedTime);

	//
	//for (auto& object : m_objects) {
	//	object.Animate(fElapsedTime);
	//}
	//for (auto& object : m_barrierObjects) {
	//	object.Animate(fElapsedTime);
	//}

	//for (auto it = m_enemyObjects.begin(); it != m_enemyObjects.end();) {
	//	if (it->m_bBlowingUp == false && it->m_bRemove == true) {
	//		m_enemyObjects.erase(it);
	//	}
	//	else {
	//		it->Animate(fElapsedTime);
	//		it++;
	//	}
	//}
	//CheckObjectByBulletCollisions();
	//CheckPlayerByWallCollision();
	//CheckPlayerByEnemyCollisions();

	//// 승리 판정 체크
	//if (m_enemyObjects.empty()) {
	//	m_gameContext.m_bWin = true;
	//	m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::RESULT);
	//}
}

void CLevel2Scene::Render(HDC hDCFrameBuffer)
{
	std::shared_ptr<CCamera> spCamera = m_spPlayer->GetCamera();
	CGraphicsPipeline::SetViewport(&spCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&spCamera->m_xmf4x4ViewPerspectiveProject);

	for (auto& object : m_objects) {
		object.Render(hDCFrameBuffer, spCamera);
	}
	for (auto& object : m_barrierObjects) {
		object.Render(hDCFrameBuffer, spCamera);
	}
	for (int i = 0; i < g_enemyCount; ++i) {
		if (m_enemyObjects[i].m_isDead)
			continue;
		m_enemyObjects[i].Render(hDCFrameBuffer, spCamera);
	}
	for (int i = 0; i < m_nBullets; ++i) {
		m_bullets[i].Render(hDCFrameBuffer, spCamera);
	}

	if (!m_spPlayer->isDead)
		m_spPlayer->Render(hDCFrameBuffer, spCamera);

	// 좌상단 (현재 HP, K/DA, gold)
	TextOutEx(hDCFrameBuffer, 0, 0, "HP: %d  K/DA (%d / %d)  gold: %d", m_spPlayer->hp,
		m_spPlayer->killCount, m_spPlayer->deathCount, m_spPlayer->gold);
	// 좌하단 (MaxHP, atk)
	TextOutEx(hDCFrameBuffer, 0, m_gameContext.m_rcClient.bottom - 20,
		"MaxHP: %d  atk: %d", m_spPlayer->maxHp, m_spPlayer->atk);
	// 우하단 상점
	TextOutEx(hDCFrameBuffer, m_gameContext.m_rcClient.right-310, m_gameContext.m_rcClient.bottom - 20,
		"1: 공격력 증가 2: 최대체력 증가 3: 체력회복");
}	

void CLevel2Scene::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0; 
		if (pKeyBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer['1'] & 0xF0) dwDirection |= INP_ONE;
		if (pKeyBuffer['2'] & 0xF0) dwDirection |= INP_TWO;
		if (pKeyBuffer['3'] & 0xF0) dwDirection |= INP_THREE;
		if (pKeyBuffer[VK_SPACE] & 0xF0) dwDirection |= INP_SPACEBAR;

		g_inputFlag.store(dwDirection);
		/*if (dwDirection)
			m_spPlayer->Move(dwDirection, 0.15f);*/
		
	}

	if (GetCapture() == m_gameContext.m_hWnd)
	{
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (pKeyBuffer[VK_LBUTTON] & 0xF0) {

				int newYaw = g_yawAngle.load() + cxMouseDelta;

				if (newYaw >= 360.f) newYaw -= 360.f;
				else if (newYaw < 0.f) newYaw += 360.f;
				g_yawAngle.store(newYaw);
				m_spPlayer->SetYawRotation(newYaw);
			}
		}
	}

	m_spPlayer->Update(m_gameContext.m_gameTimer.GetTimeElapsed());
}

void CLevel2Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	{
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		if (m_bAutoAttack == true)
			m_pLockedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_spPlayer->m_spCamera);

	}
		break;
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CLevel2Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		/*case 'A':
			m_bAutoAttack = !m_bAutoAttack;
			break;
		case 'S':
		{
			m_bShield = !m_bShield;
			if (m_bShield)
				m_spPlayer->SetColor(RGB(255, 255, 0));
			else
				m_spPlayer->SetColor(RGB(0, 0, 255));

			break;
		}
			
		case 'W':
			m_gameContext.m_bWin = true;
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::RESULT);
			break;
		case VK_ESCAPE:
			m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::MENU2);
			break;*/
		case VK_RETURN:
			break;
		case VK_SPACE:
			/*m_spPlayer->FireBullet(m_pLockedObject);
			m_pLockedObject = NULL;*/
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

}

void CLevel2Scene::Enter()
{
	m_spPlayer->SetPosition(0, 0, 0);

	// 객체 삭제 오류있음
	//CTankMesh* pTankMesh = new CTankMesh(6.f, 3.f, 6.f);
	//int i = m_enemyObjects.size();

	//while (i < 10) {
	//	m_enemyObjects.emplace_back();
	//	m_enemyObjects[i].SetMesh(pTankMesh);
	//	m_enemyObjects[i].SetColor(RGB(255, 0, 0));
	//	m_enemyObjects[i++].SetPosition(RandF(-50.f, 50.f), 0.f, RandF(10.f, 100.f));
	//}

}

void CLevel2Scene::Exit()
{
	ReleaseCapture();
}

void CLevel2Scene::CheckObjectByBulletCollisions()
{
	/*CBulletObject** ppBullets = m_spPlayer->m_ppBullets;
	for (auto& object : m_enemyObjects) {
		for (int i = 0; i < BULLETS; ++i) {
			if (ppBullets[i]->m_bActive && object.m_xmOOBB.Intersects(ppBullets[i]->m_xmOOBB))
			{
				object.m_bBlowingUp = true;
				object.m_bRemove = true;
				ppBullets[i]->Reset();
			}
		}
	}

	for (auto& object : m_barrierObjects) {
		for (int i = 0; i < BULLETS; ++i) {
			if (ppBullets[i]->m_bActive && object.m_xmOOBB.Intersects(ppBullets[i]->m_xmOOBB))
			{
				ppBullets[i]->Reset();
			}
		}
	}*/

}
void CLevel2Scene::CheckPlayerByWallCollision()
{

	BoundingOrientedBox xmOOBBPlayerMoveCheck;
	for (auto& object : m_barrierObjects) {
		if (object.m_xmOOBB.Intersects(m_spPlayer->m_xmOOBB)) {
			m_spPlayer->SetPosition(m_spPlayer->m_xmf3PrevPosition.x, m_spPlayer->m_xmf3PrevPosition.y, m_spPlayer->m_xmf3PrevPosition.z);
		}
		XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));
	}
}

void CLevel2Scene::CheckPlayerByEnemyCollisions()
{
	if (m_bShield == false) {
		for (auto& object : m_enemyObjects) {
			if (m_spPlayer->m_xmOOBB.Intersects(object.m_xmOOBB)) {
				m_gameContext.m_bWin = false;
				::ReleaseCapture();
				m_gameContext.m_sceneManager.ChangeScene(SCENE_TYPE::RESULT);
			}
		}
	} else {
		for (auto& object : m_enemyObjects) {
			if (object.m_xmOOBB.Intersects(m_spPlayer->m_xmOOBB)) {
				m_spPlayer->SetPosition(m_spPlayer->m_xmf3PrevPosition.x, m_spPlayer->m_xmf3PrevPosition.y, m_spPlayer->m_xmf3PrevPosition.z);
			}
		}
	}

}

CEnemyObject* CLevel2Scene::PickObjectPointedByCursor(int xClient, int yClient, std::shared_ptr<CCamera> spCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)spCamera->m_Viewport.m_nWidth) - 1) / spCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)spCamera->m_Viewport.m_nHeight) - 1) / spCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&spCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CEnemyObject* pNearestObject = NULL;
	for (auto& object : m_enemyObjects) {
		float fHitDistance = FLT_MAX;
		nIntersected = object.PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = &object;
		}

	}
	return(pNearestObject);
}



