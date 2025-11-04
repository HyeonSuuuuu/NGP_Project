#include "stdafx.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Player.h"
#include "MenuScene.h"
#include "AircraftGameScene.h"
#include "Level1Scene.h"
#include "Level2Scene.h"
#include "Menu2Scene.h"
#include "ResultScene.h"

void CSceneManager::Init(CGameContext& context)
{
	// 모든 Scene을 Load 해놓는다.
	/*if (!m_arrScene[(UINT)SCENE_TYPE::AIRPLANE]) {
		std::shared_ptr<CCamera> spCamera = std::make_shared<CCamera>();
		spCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		spCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
		spCamera->SetFOVAngle(60.0f);
		spCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		auto spPlayer = std::make_shared<CAirplanePlayer>();
		spPlayer->SetCamera(spCamera);
		CAirplaneMesh* pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
		spPlayer->SetPosition(0.0f, 0.0f, 0.0f);
		spPlayer->SetMesh(pAirplaneMesh);
		spPlayer->SetColor(RGB(0, 0, 255));
		spPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
		std::shared_ptr<CAirplaneGameScene> spAirScene = std::make_shared<CAirplaneGameScene>(context);
		spAirScene->SetPlayer(spPlayer);
		m_arrScene[(UINT)SCENE_TYPE::AIRPLANE] = spAirScene;
		m_arrScene[(UINT)SCENE_TYPE::AIRPLANE]->BuildObjects();
	}

	if (!m_arrScene[(UINT)SCENE_TYPE::LEVEL1]) {
		std::shared_ptr<CCamera> spCamera = std::make_shared<CCamera>();
		spCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		spCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.f, 60.f);
		spCamera->SetFOVAngle(60.f);
		spCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		auto spPlayer = std::make_shared<CLevel1Player>();
		spPlayer->SetCamera(spCamera);
		CCubeMesh* pCubeMesh = new CCubeMesh(3.f, 3.f, 6.f);
		spPlayer->SetPosition(0.f, 0.f, 0.f);
		spPlayer->SetMesh(pCubeMesh);
		spPlayer->SetColor(RGB(0, 0, 255));
		spPlayer->SetCameraOffset(XMFLOAT3(0.f, 5.f, -10.f));
		std::shared_ptr<CLevel1Scene> spL1Scene = std::make_shared<CLevel1Scene>(context);
		spL1Scene->SetPlayer(spPlayer);
		m_arrScene[(UINT)SCENE_TYPE::LEVEL1] = spL1Scene;
		m_arrScene[(UINT)SCENE_TYPE::LEVEL1]->BuildObjects();
	}*/

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

	/*if (!m_arrScene[(UINT)SCENE_TYPE::MENU]) {
		std::shared_ptr<CCamera> spCamera = std::make_shared<CCamera>();
		spCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
		spCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
		spCamera->SetFOVAngle(60.0f);
		spCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

		auto spPlayer = std::make_shared<CLevel1Player>();
		spPlayer->SetCamera(spCamera);
		CCubeMesh* pPlayer = new CCubeMesh(1.0f, 1.0f, 1.0f);
		spPlayer->SetPosition(0.0f, 0.0f, 0.0f);
		spPlayer->SetMesh(pPlayer);
		spPlayer->SetColor(RGB(0, 0, 255));
		spPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
		std::shared_ptr<CMenuScene> spMenuScene = std::make_shared<CMenuScene>(context);
		spMenuScene->SetPlayer(spPlayer);
		m_arrScene[(UINT)SCENE_TYPE::MENU] = spMenuScene;
		m_arrScene[(UINT)SCENE_TYPE::MENU]->BuildObjects();
	}*/
	if (!m_arrScene[(UINT)SCENE_TYPE::MENU2]) {
		std::shared_ptr<CMenu2Scene> spMenu2Scene = std::make_shared<CMenu2Scene>(context);
		m_arrScene[(UINT)SCENE_TYPE::MENU2] = spMenu2Scene;
		m_arrScene[(UINT)SCENE_TYPE::MENU2]->BuildObjects();
	}
	if (!m_arrScene[(UINT)SCENE_TYPE::RESULT]) {
		std::shared_ptr<CResultScene > spMenu2Scene = std::make_shared<CResultScene>(context);
		m_arrScene[(UINT)SCENE_TYPE::RESULT] = spMenu2Scene;
		m_arrScene[(UINT)SCENE_TYPE::RESULT]->BuildObjects();
	}
	m_spCurScene = m_arrScene[(UINT)SCENE_TYPE::LEVEL2];
}

void CSceneManager::ChangeScene(SCENE_TYPE type)
{
	m_spCurScene->Exit();
	m_spCurScene = m_arrScene[(size_t)type];
	m_spCurScene->Enter();
}