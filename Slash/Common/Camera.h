//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple first person style camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be constructed.  
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

class CGameObject;

class Camera
{
public:
	enum EFFECT { SHAKING, DAMAGE, ZOOMIN, ZOOMINROUND, ZOOMINROUNDULTIMATE};
	enum VIEWMODE { FIRST, TOPVIEW, DYNAMIC}; //FIRST 1ÀÎÄª
	XMFLOAT3 SaveUltimateCameraPos;
public:
	void SetViewMode(int inViewMode)
	{
		switch (inViewMode)
		{
		case FIRST:
			m_IsDynamic = true;
			bFirstPersonView = true;
			break;
		case TOPVIEW:
			bFirstPersonView = false;
			m_IsDynamic = false;
			bTestFirstPerson = false;
			break;
		case DYNAMIC:
			bFirstPersonView = false;
			m_IsDynamic = true;
			bTestFirstPerson = false;
			break;

		}
	}
	void SetCameraEffect(int CameraEffectState, CGameObject * Target = NULL)
	{
		switch (CameraEffectState)
		{
		case SHAKING: // Shaking
			bCameraEffect_Shaking = true;
			bCameraEffect_Damage = false;
			bCameraEffect_ZoomIn = false;
			bCameraEffect_ZoomIn_Round = false;

			break;
		case DAMAGE: // Damage
			bCameraEffect_Shaking = false;
			bCameraEffect_Damage = true;
			bCameraEffect_ZoomIn = false;
			bCameraEffect_ZoomIn_Round = false;

			break;
		case ZOOMIN: // ZoomIn
			bCameraEffect_Shaking = false;
			bCameraEffect_Damage = false;
			bCameraEffect_ZoomIn = true;
			bCameraEffect_ZoomIn_Round = false;

			SetZoomInTarget(Target);
			break;
		case ZOOMINROUND: // ZoomIn
			bCameraEffect_Shaking = false;
			bCameraEffect_Damage = false;
			bCameraEffect_ZoomIn = false;
			bCameraEffect_ZoomIn_Round = true;


			SetZoomInTarget(Target);
			break;
		case ZOOMINROUNDULTIMATE: // ZoomIn
			bCameraEffect_Damage = false;
			bCameraEffect_ZoomIn = false;
			bCameraEffect_ZoomIn_Round = false;
			bCameraEffect_ZoomIn_RoundUltimate = true;

			SetZoomInTarget(Target);
			break;
		}
		//CameraShakingEffect();
	} //
public:
	bool bTestFirstPerson = false;

	bool bCameraEffect_Shaking = false;
	bool bCameraEffect_Damage = false;
	bool bCameraEffect_ZoomIn = false;
	bool bCameraEffect_ZoomIn_Round = false;
	bool bCameraEffect_ZoomIn_RoundUltimate = false;

	CGameObject * Target = NULL;
public:
	float testnum = 0.0f;
	float timeLag = 0.0f;

	Camera();
	~Camera();
	bool bFirstPersonView = false;
	// Get/Set world camera position.
	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);
	
	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight()const;
	DirectX::XMFLOAT3 GetRight3f()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMFLOAT3 GetUp3f()const;
	DirectX::XMVECTOR GetLook()const;
	DirectX::XMFLOAT3 GetLook3f()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;
	
	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

	HRESULT Set_Object(CGameObject* pObj);
	void Set_Dynamic(bool _isDynamic) { m_IsDynamic = _isDynamic; }

	int Update();

	void CameraEffect_Shaking();
	void CameraEffect_Damage();
	void CameraEffect_ZoomIn();
	void CameraEffect_ZoomIn_Round();
	void CameraEffect_ZoomIn_RoundUltimate();

	void SetViewMatrix(DirectX::XMFLOAT4X4 inView);

	void SetZoomInTarget(CGameObject * InTarget)
	{
		Target = InTarget;
	}

private:

	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

	// Cache frustum properties.
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool mViewDirty = true;



	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	CGameObject*			m_pObject;

	bool					m_IsDynamic;
	bool bSaveUltimateCameraPosTest = false;

	float LastLength = 10000.0f;
};

#endif // CAMERA_H