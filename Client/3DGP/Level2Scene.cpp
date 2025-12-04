#include "stdafx.h"
#include "Level2Scene.h"
#include "Player.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"
#include "GameContext.h"
#include "Globals.h"
#include <vector>
#include <algorithm>

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
	// Globals.h ̿ؼ Update
	EnterCriticalSection(&g_csPlayers);
	g_enemyCount = g_players.size()-1;

	int j = 0;
	for (int i = 0; i < g_enemyCount+1; ++i) {
		if (g_players[i].id == g_myId) {
			m_spPlayer->isDead = g_players[i].isDead;
			if (m_spPlayer->isDead)
				continue;
			/*m_spPlayer->SetPosition(g_players[i].x, 0, g_players[i].z);*/
			// Target position 
			m_spPlayer->m_targetX = g_players[i].x;
			m_spPlayer->m_targetZ = g_players[i].z;
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
	m_nBullets = g_bullets.size();

	for (int i = 0; i < m_nBullets; ++i) {
		m_bullets[i].SetPosition(g_bullets[i].x, 0.f, g_bullets[i].z);
	}
	LeaveCriticalSection(&g_csBullets);

	m_spPlayer->Animate(fElapsedTime);
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

	EnterCriticalSection(&g_csPlayers);
	std::vector<PlayerInfo> sortedPlayers = g_players;
	LeaveCriticalSection(&g_csPlayers);

	std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](const PlayerInfo& a, const PlayerInfo& b) {
		return a.killCount > b.killCount;
		});

	TextOutEx(hDCFrameBuffer, m_gameContext.m_rcClient.right - 150, 0, "Leaderboard (K/DA)");
	int yPos = 20;
	for (const auto& player : sortedPlayers) {
		TextOutEx(hDCFrameBuffer, m_gameContext.m_rcClient.right - 150, yPos, "%d: (%d / %d)", player.id, player.killCount, player.deathCount);
		yPos += 20;
	}

	// » ( HP, K/DA, gold)
	TextOutEx(hDCFrameBuffer, 0, 0, "HP: %d  K/DA (%d / %d)  gold: %d", m_spPlayer->hp,
		m_spPlayer->killCount, m_spPlayer->deathCount, m_spPlayer->gold);
	// 스탯 (MaxHP, atk)
	TextOutEx(hDCFrameBuffer, 0, m_gameContext.m_rcClient.bottom - 20,
		"MaxHP: %d  atk: %d", m_spPlayer->maxHp, m_spPlayer->atk);
	// 업그레이드 옵션
	TextOutW(hDCFrameBuffer, m_gameContext.m_rcClient.right-310, m_gameContext.m_rcClient.bottom - 20,
		L"1: 공격력  2: 최대체력  3: 체력회복", 24);
	// 중앙 킬보드
	int offset = 0;
	EnterCriticalSection(&g_csKillEvents);
	int j = 0;
	wchar_t killFeedBuffer[100];
	for (auto it = g_killEvents.begin(); it != g_killEvents.end();) {
		it->displayTime -= 1;
		if (it->displayTime <= 0)
			it = g_killEvents.erase(it);
		else {
			wsprintfW(killFeedBuffer, L"킬: %d -> %d", it->killerId, it->killedId);
			TextOutW(hDCFrameBuffer, m_gameContext.m_rcClient.right / 2, 0 + j++ * 20, killFeedBuffer, wcslen(killFeedBuffer));
			++it;
		}
	}
	LeaveCriticalSection(&g_csKillEvents);
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
		if (dwDirection)
			m_spPlayer->Move(dwDirection, 0.1f); //  30FPS에서0.2이므로 0.1
		
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

void CLevel2Scene::Enter()
{
	m_spPlayer->SetPosition(0, 0, 0);
}

void CLevel2Scene::Exit()
{
	ReleaseCapture();
}