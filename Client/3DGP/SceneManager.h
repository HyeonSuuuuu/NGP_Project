#pragma once
#include "Scene.h"

class CGameContext;


enum class SCENE_TYPE {
	NONE,
	MENU,
	MENU2,
	AIRPLANE,
	LEVEL1,
	LEVEL2,
	RESULT,
	END,
};


class CSceneManager {
public:
	CSceneManager() {}
	~CSceneManager() {}
	void Init(CGameContext& context);

	std::shared_ptr<CScene> GetCurScene() { return m_spCurScene; }
	void ChangeScene(SCENE_TYPE type);


private:
	std::array<std::shared_ptr<CScene>, (size_t)SCENE_TYPE::END>	m_arrScene;
	std::shared_ptr<CScene>											m_spCurScene;
};
