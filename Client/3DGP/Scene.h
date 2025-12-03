#pragma once
#include "stdafx.h"
// object set할게 많아서 나중에
//enum class GROUP_TYPE {
//	DEFAULT,
//	PLAYER,
//	WALL,
//	END = 32,
//};

// SceneManager > ChangeScene SCENE_ID를 보고 알맞은 SCENE을 생성 (만약 상태를 보관한다면 delete를 안하면 될것) 씬메니저까지 필요 없다.

class CGameContext;




class CScene
{
public:
	CScene(CGameContext& context)
		: m_gameContext(context)
	{}
	virtual ~CScene() {}

	virtual void BuildObjects() = 0;
	virtual void ReleaseObjects() = 0;

	virtual void Animate(float fElapsedTime) {}
	virtual void Render(HDC hDCFrameBuffer) = 0;

	virtual void Enter() {}
	virtual void Exit() {}

	virtual void ProcessInput() {}
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}

	void SetName(const std::wstring& strName) { m_strName = strName; }
	const std::wstring& GetName() { return m_strName; }

protected:
	CGameContext& m_gameContext;


private:
	// 디버깅용
	std::wstring m_strName;
};











