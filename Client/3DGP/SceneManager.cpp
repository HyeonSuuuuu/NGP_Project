#include "stdafx.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Player.h"
#include "Level2Scene.h"


void CSceneManager::Init(CGameContext& context)
{
	// 모든 Scene을 Load 해놓는다.

	if (!m_arrScene[(UINT)SCENE_TYPE::LEVEL2]) {
		std::shared_ptr<CCamera> spCamera = std::make_shared<CCamera>();
		spCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		spCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.f, 60.f);
		spCamera->SetFOVAngle(60.f);
		spCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		auto spPlayer = std::make_shared<CLevel2Player>();
		spPlayer->SetCamera(spCamera);
		CTankMesh* pTankMesh = new CTankMesh(6.f, 3.f, 6.f);
		spPlayer->SetPosition(0.f, 0.f, 0.f);
		spPlayer->SetMesh(pTankMesh);
		spPlayer->SetColor(RGB(0, 0, 255));
		spPlayer->SetCameraOffset(XMFLOAT3(0.f, 5.f, -10.f));
		std::shared_ptr<CLevel2Scene> spL2Scene = std::make_shared<CLevel2Scene>(context);
		spL2Scene->SetPlayer(spPlayer);
		m_arrScene[(UINT)SCENE_TYPE::LEVEL2] = spL2Scene;
		m_arrScene[(UINT)SCENE_TYPE::LEVEL2]->BuildObjects();
	}
	m_spCurScene = m_arrScene[(UINT)SCENE_TYPE::LEVEL2];
}

void CSceneManager::ChangeScene(SCENE_TYPE type)
{
	m_spCurScene->Exit();
	m_spCurScene = m_arrScene[(size_t)type];
	m_spCurScene->Enter();
}