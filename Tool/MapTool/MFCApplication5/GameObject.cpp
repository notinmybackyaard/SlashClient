#include "stdafx.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Camera.h"
#include "Transform.h"

CGameObject* CGameObject::m_pAllObject[MAXOBJECTID] = { nullptr };
unsigned long CGameObject::m_iAllObjectIndex = 0;

CGameObject::CGameObject(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize)
	: m_d3dDevice(d3dDevice)
	, mSrvDescriptorHeap(srv)
	, mCbvSrvDescriptorSize(srvSize)
{
	//m_pTransCom = 
	while (m_pAllObject[m_iAllObjectIndex++])
	{
		m_iAllObjectIndex %= MAXOBJECTID;
	}
	m_pAllObject[m_iAllObjectIndex] = this;
	m_iMyObjectID = m_iAllObjectIndex;
}


CGameObject::~CGameObject()
{
	m_pAllObject[m_iMyObjectID] = nullptr;
}


bool CGameObject::Update(const GameTimer & gt)
{
	m_pFrameResource = CObjectManager::GetInstance()->GetCurFrameResource();
	m_pMesh->Update(gt);

	return true;
}

void CGameObject::Render(ID3D12GraphicsCommandList* cmdList)
{

}

HRESULT CGameObject::Initialize()
{
	return S_OK;
}

void CGameObject::RenderBounds(ID3D12GraphicsCommandList * cmdList)
{
}

void CGameObject::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransCom);
}

void CGameObject::BuildOOBBRenderer(BoundingOrientedBox m_xmOOBB)
{
	
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(World._41, World._42, World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(World._31, World._32, World._33)));

}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(World._21, World._22, World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(World._11, World._12, World._13)));
}

void CGameObject::SetPosition(float x, float y, float z)
{
	World._41 = x;
	World._42 = y;
	World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	World = Matrix4x4::Multiply(mtxRotate, World);
}

void CGameObject::Rotate(XMFLOAT3 * pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	World = Matrix4x4::Multiply(mtxRotate, World);
}

void CGameObject::Animate(const GameTimer & gt)
{
	// Animate

}



void AnimateStateMachine::AnimationStateUpdate(const GameTimer & gt)
{
	if (bTimerIdle == true)
	{
		m_fAnimationKeyFrameIndex += gt.DeltaTime() * 25;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex;
		if (m_fAnimationKeyFrameIndex > (*vecAnimFrame)[0])
		{
			bTimerIdle = false;

			m_fAnimationKeyFrameIndex = 0;
		}
	}


	if (bTimerWalk == true)
	{

		m_fAnimationKeyFrameIndex_Walk += gt.DeltaTime() * 45;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Walk;
		if (m_fAnimationKeyFrameIndex_Walk > (*vecAnimFrame)[1])
		{
			bTimerWalk = false;
			m_fAnimationKeyFrameIndex_Walk = 0;
		}

	}


	if (bTimerAttack1 == true)
	{

		m_fAnimationKeyFrameIndex_Attack1 += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack1;
		if (m_fAnimationKeyFrameIndex_Attack1 > (*vecAnimFrame)[2])
		{
			bTimerAttack1 = false;
			m_fAnimationKeyFrameIndex_Attack1 = 0;
		}

	}


	if (bTimerAttack2 == true)
	{

		m_fAnimationKeyFrameIndex_Attack2 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;
		if (m_fAnimationKeyFrameIndex_Attack2 > (*vecAnimFrame)[3])
		{
			bTimerAttack2 = false;
			m_fAnimationKeyFrameIndex_Attack2 = 0;
		}


	}



	if (bTimerAttack3 == true)
	{

		m_fAnimationKeyFrameIndex_Attack3 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (m_fAnimationKeyFrameIndex_Attack3 > (*vecAnimFrame)[4])
		{
			bTimerAttack3 = false;
			m_fAnimationKeyFrameIndex_Attack3 = 0;
		}

	}

	if (bTimerDead == true)
	{
		//cout << m_fAnimationKeyFrameIndex_Dead << endl;
		if (m_bIsLife == true)
			m_fAnimationKeyFrameIndex_Dead += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (m_fAnimationKeyFrameIndex_Dead + 1> (*vecAnimFrame)[5])
		{
			m_bIsLife = false;
			bTimerDead = false;
			//m_fAnimationKeyFrameIndex_Dead = 0;
		}

	}




}

void AnimateStateMachine::SetTimerTrueFalse()
{
	//
	//
	//
	//

	//

	if (m_iAnimState == IdleState)
	{
		bTimerIdle = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex;
	}
	if (m_iAnimState == WalkState)
	{
		bTimerWalk = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Walk;
	}
	if (m_iAnimState == Attack1State)
	{
		bTimerAttack1 = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack1;
	}
	if (m_iAnimState == Attack2State)
	{
		bTimerAttack2 = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;
	}
	if (m_iAnimState == Attack3State)
	{
		bTimerAttack3 = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;
	}

	if (m_iAnimState == DeadState)
	{
		bTimerDead = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Dead;
	}
}

void AnimateStateMachine::Free()
{
}
