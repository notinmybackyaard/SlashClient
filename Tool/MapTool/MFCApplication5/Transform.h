#pragma once

#include "Component.h"
#include "Define.h"

class CGameObject;

class CTransform
	: public CComponent
{
private:
	explicit CTransform(CGameObject* pObj);
public:
	virtual ~CTransform();

public:
	HRESULT				Initialize();
	virtual void		Update_Component(const GameTimer& gt);


	// Get / Set
	// Get / Set
	inline XMFLOAT4X4&			GetWorld() { return m_f4x4World; }

	inline XMFLOAT3&			GetPosition() { return m_f3Position; }
	inline XMFLOAT3&			GetScale() { return m_f3Scale; }
	inline XMFLOAT3&			GetRotation() { return m_f3Rotation; }

	inline XMFLOAT3&			GetRight() { XMFLOAT3 right; memcpy(&right, &m_f4x4World._11, sizeof(XMFLOAT3)); return XMFloat3Normalize(right); }
	inline XMFLOAT3&			GetUp() { XMFLOAT3 up; memcpy(&up, &m_f4x4World._21, sizeof(XMFLOAT3)); return XMFloat3Normalize(up); }
	inline XMFLOAT3&			GetLook() { XMFLOAT3 look; memcpy(&look, &m_f4x4World._31, sizeof(XMFLOAT3)); return XMFloat3Normalize(look); }

	inline void					SetParentMatrix(XMFLOAT4X4* pf4x4Parent) { if (nullptr == pf4x4Parent) return; m_pf4x4ParentWorld = pf4x4Parent; }

	// 이동
	inline void					Translate(const XMFLOAT3& f3Trans) { m_f3Position += f3Trans; }
	inline void					Translate(const XMVECTOR& vTrans) { m_f3Position += vTrans; }
	inline void					Translate(const float x, const float y, const float z) { m_f3Position.x += x; m_f3Position.y += y; m_f3Position.z += z; }

	inline void					Translation(const XMFLOAT3& f3Trans) { m_f3Position = f3Trans; }
	inline void					Translation(const XMVECTOR& vTrans) { XMStoreFloat3(&m_f3Position, vTrans); }
	inline void					Translation(const float x, const float y, const float z) { m_f3Position.x = x; m_f3Position.y = y; m_f3Position.z = z; }

	// 크기
	inline void					Scale(const XMFLOAT3& f3Scale) { m_f3Scale += f3Scale; }
	inline void					Scale(const XMVECTOR& vScale) { m_f3Scale += vScale; }
	inline void					Scale(const float x, const float y, const float z) { m_f3Scale.x += x; m_f3Scale.y += y; m_f3Scale.z += z; }

	inline void					Scaling(const XMFLOAT3& f3Scale) { m_f3Scale = f3Scale; }
	inline void					Scaling(const XMVECTOR& vScale) { XMStoreFloat3(&m_f3Scale, vScale); }
	inline void					Scaling(const float x, const float y, const float z) { m_f3Scale.x = x; m_f3Scale.y = y; m_f3Scale.z = z; }

	// 회전
	inline void					Rotate(const XMFLOAT3& f3Rot) { m_f3Rotation += f3Rot; }
	inline void					Rotate(const XMVECTOR& vRot) { m_f3Rotation += vRot; }
	inline void					Rotate(const float x, const float y, const float z) { m_f3Rotation.x += x; m_f3Rotation.y += y; m_f3Rotation.z += z; }

	inline void					Rotation(const XMFLOAT3& f3Rot) { m_f3Rotation = f3Rot; }
	inline void					Rotation(const XMVECTOR& vRot) { XMStoreFloat3(&m_f3Rotation, vRot); }
	inline void					Rotation(const float x, const float y, const float z) { m_f3Rotation.x = x; m_f3Rotation.y = y; m_f3Rotation.z = z; }

private:
	CGameObject*		m_pObject;

	XMFLOAT4X4			m_f4x4World;
	XMFLOAT4X4*			m_pf4x4ParentWorld;

	XMFLOAT3			m_f3Position;
	XMFLOAT3			m_f3Scale;
	XMFLOAT3			m_f3Rotation;

public:
	virtual CComponent*		Clone();
	static CTransform*		Create(CGameObject* pObj);

private:
	virtual void Free();
};