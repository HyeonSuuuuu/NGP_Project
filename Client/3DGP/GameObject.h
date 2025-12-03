#pragma once

#include "Mesh.h"
#include "Camera.h"

class CGameObject
{
public:
	CGameObject() { }
	virtual ~CGameObject();

public:
	bool						m_bActive = true;

	CMesh*						m_pMesh = NULL;
	XMFLOAT4X4					m_xmf4x4World = Matrix4x4::Identity();

	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();

	CGameObject*				m_pObjectCollided = NULL;
	DWORD						m_dwColor = RGB(255, 0, 0);

	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;

	XMFLOAT3					m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float						m_fRotationSpeed = 0.0f;

public:
	void SetActive(bool bActive) { m_bActive = bActive; }
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }

	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }

	void SetRotationTransform(XMFLOAT4X4 *pmxf4x4Transform);

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void UpdateBoundingBox();

	void Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh);

	virtual void OnUpdateTransform() { }
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);

	void GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection);
	int PickObjectByRayIntersection(XMVECTOR& xmPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance);
};

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool						m_bBlowingUp = false;

	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;
	float						m_fExplosionSpeed = 10.0f;
	float						m_fExplosionRotation = 720.0f;

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);
	void Render(HDC hDCFrameBuffer);
public:
	static CMesh*				m_pExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion();
};

class CEnemyObject : public CExplosiveObject {
public:
	CEnemyObject() {}
	virtual ~CEnemyObject() {}
	void SetYawRotation(float yawAngle);

	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	bool m_isDead = false;

};

class CBarrierObject : public CGameObject {
public:
	CBarrierObject(CCubeMesh* pCubeMesh, float x, float y, float z)
	{
		this->SetMesh(pCubeMesh);
		this->SetColor(RGB(0, 255, 0));
		this->SetPosition(x, y, z);
	}
	// 복사 생성자: base 멤버는 CGameObject(other)로 복사되고,
	// 그 뒤 m_pMesh에 대해 참조를 증가시킨다.
	CBarrierObject(const CBarrierObject& other) : CGameObject(other)
	{
		if (m_pMesh) m_pMesh->AddRef();
	}

	// 복사 대입: 기존 소유 해제 후 base 할당, 새 소유 참조 증가
	CBarrierObject& operator=(const CBarrierObject& other)
	{
		if (this != &other)
		{
			CGameObject::operator=(other);      // base 멤버 복사 (m_pMesh도 복사됨)
			if (m_pMesh) m_pMesh->AddRef();     // 새 소유에 대해 증가
		}
		return *this;
	}
	virtual ~CBarrierObject() {}

};


class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	BoundingOrientedBox			m_xmOOBBPlayerMoveCheck = BoundingOrientedBox();
	XMFLOAT4					m_pxmf4WallPlanes[6];

	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject() = default;
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(float fElapsedTime);

	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingDelayTime = 0.3f;
	float						m_fLockingTime = 4.0f;
	CGameObject*				m_pLockedObject = NULL;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void Reset();
};

class CAxisObject : public CGameObject
{
public:
	CAxisObject() { }
	virtual ~CAxisObject() { }

	virtual void Render(HDC hDCFrameBuffer, std::shared_ptr<CCamera> spCamera);
};


