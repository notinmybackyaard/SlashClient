#include "stdafx.h"
#include "Transform.h"
//#include "GameObject.h"

CTransform::CTransform(CGameObject* pObj)
	: m_pObject(pObj)
	, m_pf4x4ParentWorld(nullptr)
{
	ZeroMemory(&m_f3Position, sizeof(XMFLOAT3));
	ZeroMemory(&m_f3Rotation, sizeof(XMFLOAT3));
	m_f3Scale = XMFLOAT3(1.f, 1.f, 1.f);

	XMStoreFloat4x4(&m_f4x4World, XMMatrixIdentity());

	//m_f3Position.x;
	//m_f3Position = XMFLOAT3(1, 2, 3);
	//XMVECTOR v;

	//v.x
	//v = XMVECTOR(1.f, 2.f, 3.f, 4.f);
}

CTransform::~CTransform()
{
}

HRESULT CTransform::Initialize()
{
	return S_OK;
}

void CTransform::Update_Component(const GameTimer & gt)
{
	XMMATRIX matWorld;
	XMVECTOR vScale, vRot, vTrans;
	vScale = XMLoadFloat3(&m_f3Scale);
	vRot = XMLoadFloat3(&m_f3Rotation);
	vTrans = XMLoadFloat3(&m_f3Position);
	vRot = XMQuaternionRotationRollPitchYaw(m_f3Rotation.x, m_f3Rotation.y, m_f3Rotation.z);

	XMFLOAT3 f3Default = XMFLOAT3(1.f, 1.f, 1.f);
	XMVECTOR vDefault = XMLoadFloat3(&f3Default);
	
	XMStoreFloat4x4(&m_f4x4World, XMMatrixScaling(m_f3Scale.x, m_f3Scale.y, m_f3Scale.z)
		* XMMatrixRotationX(XMConvertToRadians(m_f3Rotation.x)) * XMMatrixRotationY(XMConvertToRadians(m_f3Rotation.y)) * XMMatrixRotationZ(XMConvertToRadians(m_f3Rotation.z))
		*XMMatrixTranslation(m_f3Position.x, m_f3Position.y, m_f3Position.z));

	//if (m_pf4x4ParentWorld != nullptr)
	//	m_f4x4World *= *m_pf4x4ParentWorld;
}

CComponent * CTransform::Clone()
{
	return new CTransform(*this);
}

CTransform * CTransform::Create(CGameObject * pObj)
{
	CTransform* pInstance = new CTransform(pObj);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CTransform Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
}
