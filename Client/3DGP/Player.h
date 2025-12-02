#pragma once

#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();
public:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3PrevPosition = m_xmf3Position;
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	std::shared_ptr<CCamera>	m_spCamera = nullptr;

public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetYawRotation(float yawAngle);

	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);

	void Update(float fTimeElapsed = 0.016f);

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);

	void SetCamera(std::shared_ptr<CCamera> spCamera) { m_spCamera = spCamera; }
	std::shared_ptr<CCamera> GetCamera() { return(m_spCamera); }
};

#define BULLETS					50

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer();
	virtual ~CAirplanePlayer();

	float						m_fBulletEffectiveRange = 150.0f;
	CBulletObject*				m_ppBullets[BULLETS];

	void FireBullet(CGameObject* pLockedObject);

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);
};


class CLevel1Player : public CPlayer
{
public:
	CLevel1Player();
	virtual ~CLevel1Player();

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);
	
	void AddPoint(const XMFLOAT3& point)
	{
		m_points.push_back(point);
	}

	XMFLOAT3 GetPosition(float t) const;
	XMFLOAT3 GetTangent(float t) const;

	bool isPathEnd();

private:
	std::vector<XMFLOAT3>		m_points;
	float						m_fPathPosition = 0.f;
	float						m_fPathSpeed = 0.1f;
};

class CLevel2Player : public CPlayer
{
public:
	CLevel2Player();
	virtual ~CLevel2Player();

	float						m_fBulletEffectiveRange = 150.0f;
	CBulletObject*				m_ppBullets[BULLETS];
	float						m_fBarrelLength = 10.f;
	
	void FireBullet(CGameObject* pLockedObject);

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);


	int maxHp = 0;
	int hp = 0;
	int atk = 0;
	int gold = 0;

	bool isDead = false;
	uint16_t killCount = 0;
	uint16_t deathCount = 0;
};