//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "../Chapter 16 Instancing and Frustum Culling/InstancingAndCulling/stdafx.h"
#include "Camera.h"
#include "../Chapter 16 Instancing and Frustum Culling/InstancingAndCulling/GameObject.h"
#include "../Chapter 16 Instancing and Frustum Culling/InstancingAndCulling/InputDevice.h"
#include "../Chapter 16 Instancing and Frustum Culling/InstancingAndCulling/Management.h"
#include "../Chapter 16 Instancing and Frustum Culling/InstancingAndCulling/Player.h"

Camera::Camera()
	: m_pObject(nullptr)
	, m_IsDynamic(true)
{
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
	SaveUltimateCameraPos.x = GetPosition3f().x;
	SaveUltimateCameraPos.y = 50.f;
	SaveUltimateCameraPos.z = GetPosition3f().z;
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPosition()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition3f()const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	mViewDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
	mViewDirty = true;
}

XMVECTOR Camera::GetRight()const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight3f()const
{
	return mRight;
}

XMVECTOR Camera::GetUp()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp3f()const
{
	return mUp;
}

XMVECTOR Camera::GetLook()const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook3f()const
{
	return mLook;
}

float Camera::GetNearZ()const
{
	return mNearZ;
}

float Camera::GetFarZ()const
{
	return mFarZ;
}

float Camera::GetAspect()const
{
	return mAspect;
}

float Camera::GetFovY()const
{
	return mFovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mViewDirty = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mViewDirty = true;
}

XMMATRIX Camera::GetView()const
{
	assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProj()const
{
	return XMLoadFloat4x4(&mProj);
}


XMFLOAT4X4 Camera::GetView4x4f()const
{
	assert(!mViewDirty);
	return mView;
}

XMFLOAT4X4 Camera::GetProj4x4f()const
{
	return mProj;
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));

	mViewDirty = true;
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp,   XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight,   XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::UpdateViewMatrix()
{	
	if(mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = false;
	}
}

HRESULT Camera::Set_Object(CGameObject * pObj)
{
	if (nullptr == pObj)
		return E_FAIL;

	m_pObject = pObj;

	return S_OK;
}

int Camera::Update()
{
	if (KeyBoard_Input(DIK_F3) == CInputDevice::INPUT_DOWN)
	{
		SetViewMode(DYNAMIC);
	}
	if (KeyBoard_Input(DIK_F4) == CInputDevice::INPUT_DOWN)
	{
		SetViewMode(TOPVIEW);

	}
	if (KeyBoard_Input(DIK_F5) == CInputDevice::INPUT_DOWN)
	{
		SetViewMode(FIRST);

	
	}

	if (!m_IsDynamic)
	{
		if (nullptr == m_pObject)
			return 0;
		XMFLOAT4X4 world = m_pObject->GetWorld();
		XMFLOAT3 vPos, vLook;
		memcpy(&vLook, &world._41, sizeof(XMFLOAT3));
		vPos.x = vLook.x;// + 10.f;
		vPos.y = vLook.y + 20.f;
		vPos.z = vLook.z - 20.f;

		LookAt(vPos, vLook, XMFLOAT3(0, 1, 0));
	}
	if (m_IsDynamic && bFirstPersonView)
	{
		if (nullptr == m_pObject)
			return 0;
		XMFLOAT4X4 world = m_pObject->GetWorld();
		XMFLOAT3 vPos, vLook;
		
		vLook = XMFLOAT3((m_pObject->GetPosition().x - m_pObject->GetUp().x), (m_pObject->GetPosition().y + 1.7 - m_pObject->GetUp().y), (m_pObject->GetPosition().z - m_pObject->GetUp().z));
		//memcpy(&vLook, &world._21, sizeof(XMFLOAT3));
		vPos.x = m_pObject->GetPosition().x;// + 10.f;
		vPos.y = m_pObject->GetPosition().y + 1.7;
		vPos.z = m_pObject->GetPosition().z ;
		
		SetPosition(vPos);
		if (!bTestFirstPerson)
		{
			LookAt(vPos, vLook, XMFLOAT3(0, 1, 0));
			bTestFirstPerson = true;

		}
	}

	UpdateViewMatrix();

	return 0;
}

void Camera::CameraEffect_Shaking()
{

	if (bCameraEffect_Shaking)
	{
		auto * m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player");
		//cout << dynamic_cast<Player*>(m_pPlayer)->bIsUltimateState << endl;

		if (this != NULL)
		{
			m_IsDynamic = true;
			if (!bFirstPersonView && !(dynamic_cast<Player*>(m_pPlayer)->bIsUltimateState)) //3인칭 모드
			{
				RotateY(0.01 *  sin(testnum * 5.f));
				Pitch(0.01 *  sin(testnum * 5.f));

				SetPosition(m_pObject->GetPosition().x, m_pObject->GetPosition().y + 20, m_pObject->GetPosition().z - 20);

			}
			else if ((dynamic_cast<Player*>(m_pPlayer)->bIsUltimateState))
			{
			
				//SetPosition(pos);
			}
			else
			{
				RotateY(0.05 *  sin(testnum * 5.f));
				Pitch(0.05 *  sin(testnum * 5.f));
				SetPosition(m_pObject->GetPosition().x, m_pObject->GetPosition().y + 1.7, m_pObject->GetPosition().z);

			}


			if (testnum < 4.0f * 3.14f / 5.f)
				testnum += 0.1f;
			else
			{
				if (!bFirstPersonView) // TopView에선 카메라 고정시켜야함
					m_IsDynamic = false;

				bCameraEffect_Shaking = false;
				testnum = 0.0f;
			}

		}
	}
	
}

void Camera::CameraEffect_Damage()
{

	if (bCameraEffect_Damage)
	{
		if (this != NULL)
		{
			m_IsDynamic = true;
			if (bFirstPersonView) //3인칭 모드
			{
				RotateY(0.05 *  sin(-testnum * 5.f));
				Pitch(0.05 *  sin(-testnum * 5.f));
				SetPosition(m_pObject->GetPosition().x, m_pObject->GetPosition().y + 1.7, m_pObject->GetPosition().z);

			}

			if (testnum < 2.0f * 3.14f / 5.f)
				testnum += 0.01f;
			else
			{
				if (!bFirstPersonView) //3인칭 모드
					m_IsDynamic = false;
				else
					m_IsDynamic = true;


				bCameraEffect_Damage = false;
				testnum = 0.0f;
			}

		}
	}
}

void Camera::CameraEffect_ZoomIn()
{

	if (bCameraEffect_ZoomIn)
	{
		if (this != NULL)
		{
			m_IsDynamic = true;

			XMFLOAT3 dir = XMFLOAT3(Target->GetPosition().x - GetPosition3f().x, Target->GetPosition().y - GetPosition3f().y, Target->GetPosition().z - GetPosition3f().z);
			XMFLOAT3 normalizeDir;
			XMStoreFloat3(&normalizeDir, XMVector3Normalize(XMLoadFloat3(&dir)));

			XMFLOAT3 pos = XMFLOAT3(Target->GetPosition().x, Target->GetPosition().y +40, Target->GetPosition().z);
			XMFLOAT3 movePos = XMFLOAT3(GetPosition3f().x + normalizeDir.x * testnum * 0.01, GetPosition3f().y + normalizeDir.y * testnum* 0.01, GetPosition3f().z + normalizeDir.z * testnum* 0.01);

			LookAt(movePos, pos, XMFLOAT3(0, 1, 0));
			if (sqrt(pow(pos.x - GetPosition3f().x,2) + pow(pos.y - GetPosition3f().y, 2)+ pow(pos.z - GetPosition3f().z, 2))  > 50.0f)
				testnum += 0.1f;
			else
			{
				Target = NULL;
				bCameraEffect_ZoomIn = false;
				testnum = 0.0f;
				SetViewMode(FIRST);
			}
		}

		

	}
}
void Camera::CameraEffect_ZoomIn_Round()
{

	if (bCameraEffect_ZoomIn_Round)
	{
		if (this != NULL && !bFirstPersonView)
		{
			m_IsDynamic = true;

			XMFLOAT3 pos = XMFLOAT3(Target->GetPosition().x, Target->GetPosition().y, Target->GetPosition().z);
			XMFLOAT3 movePos = XMFLOAT3(15.0f * cos(testnum) + Target->GetPosition().x, Target->GetPosition().y + 10.0f + Target->GetPosition().y, 15.0f * sin(testnum) + Target->GetPosition().z);
			LookAt(movePos, Target->GetPosition(), XMFLOAT3(0, 1, 0));
		}

		if (testnum < 3.14f) //180도 회전
			testnum += 0.06f;
		else
		{
			Target = NULL;
			bCameraEffect_ZoomIn_Round = false;
			testnum = 0.0f;
			m_IsDynamic = false;
			
		}

	}
}

void Camera::CameraEffect_ZoomIn_RoundUltimate()
{
	auto * m_pPlayer = dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player"));

	if (bCameraEffect_ZoomIn_RoundUltimate)
	{
		if (this != NULL && !bFirstPersonView )
		{
			m_IsDynamic = true;

			XMFLOAT3 pos = XMFLOAT3(Target->GetPosition().x, Target->GetPosition().y, Target->GetPosition().z);
			XMFLOAT3 movePos{};
			movePos = XMFLOAT3(10.0f * cos(testnum) + Target->GetPosition().x, Target->GetPosition().y + 5.0f + Target->GetPosition().y, 10.0f * sin(testnum) + Target->GetPosition().z);
			LookAt(movePos, Target->GetPosition(), XMFLOAT3(0, 1, 0));

		}

		if (m_pPlayer->GetAnimateMachine()->bTimerUltimate) //180도 회전
		{
				testnum += 0.06f;
		}
		else //궁 끝나면
		{
			Target = NULL;
			bCameraEffect_ZoomIn_RoundUltimate = false;
			testnum = 0.0f;
			m_IsDynamic = false;
			timeLag = 0.0f;
			bSaveUltimateCameraPosTest = false;
			LastLength = 0.0f;
		}
	}
}


void Camera::SetViewMatrix(DirectX::XMFLOAT4X4 inView)
{
	XMMATRIX mtxInView = XMLoadFloat4x4(&inView);


}


