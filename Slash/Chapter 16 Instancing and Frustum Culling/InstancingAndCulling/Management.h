#pragma once

#include "Base.h"
#include "Define.h"
#include "GameTimer.h"
#include "Camera.h"
#include "Scene.h"
#include "Sound.h"

class CRenderer;
class ChangeUI;
class NumUI_Inst;

class CManagement
	: public CBase
{
	enum SCENETYPE{SCENE_START, SCENE_SELECT, SCENE_STAGE, SCENE_END};

	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual ~CManagement();

public:
	vector<ComPtr<ID3D12DescriptorHeap>>&	GetSrvDescriptorHeap() { return mSrvDescriptorHeap; }
	UINT&							GetCbvSrvDescriptorSize() { return mCbvSrvDescriptorSize; }
	FrameResource*					GetCurFrameResource() { return mCurrFrameResource; }
	CRenderer*						GetRenderer() { return m_pRenderer; }
	CSound*							GetSound() { return m_pSound; }
	CScene*							Get_CurScene() { return m_pCurScene; }

	bool&							Get_IsStop() { return m_IsStop; }
public:
	Camera*			Get_MainCam() { return m_pCurScene->Get_MainCam(); }
	HRESULT			Set_MainCam(Camera* pCam) { return m_pCurScene->Set_MainCam(pCam); }
	HRESULT			Set_CamFrustum(DirectX::BoundingFrustum* pFrustum) { return m_pCurScene->Set_CamFrustum(pFrustum); }
	DirectX::BoundingFrustum* Get_CamFrustum() { return m_pCurScene->Get_CamFrustum(); }

	auto&			Get_MapLayer() { return m_pCurScene->Get_MapLayer(); }
	CLayer*			Get_Layer(wchar_t* pLayerTag);
	CGameObject*	Find_Object(wchar_t* LayerTag, unsigned int iIdx = 0);


	HRESULT			Add_NumUI(int iNum, XMFLOAT3 f3Pos, bool isCritical = false);

	void			SetExp(CGameObject* pObj, float _exp);
	void			AddExp(CGameObject* pObj, float _exp);

	void			SetLevel(CGameObject* pObj, int iLv);

	void			PlayLevelUP();
	void			SetLevelUPUI(vector<ChangeUI*> vec);

	void			PlaySkillUI(int inum);
	void			SetSkillUPUI(vector<ChangeUI*> vec);
private:
	CScene*				m_pCurScene;
	CScene*				m_pChangeScene = nullptr;
	CRenderer*			m_pRenderer;
	CSound*				m_pSound;

	NumUI_Inst*			m_pNumUI;

	vector<ChangeUI*>	m_pLevelUP;
	vector<ChangeUI*>	m_pSkillReady;

	vector<ComPtr<ID3D12DescriptorHeap>> mSrvDescriptorHeap;
	UINT mCbvSrvDescriptorSize = 0;

	FrameResource* mCurrFrameResource = nullptr;

	SCENETYPE			m_tCurScene;

	bool				m_IsStop = false;
	bool				m_IsChange = false;
public:
	HRESULT		Init_Management(CRenderer* pRenderer, NumUI_Inst* pNumUI);
	bool		Update(const GameTimer& gt, const FrameResource* pCruRrc);
	void		Render(ID3D12GraphicsCommandList* cmdList);

public:
	HRESULT		Change_Scene(CScene* pScene);
	HRESULT		Set_Sound(CSound* pSound);

private:
	virtual void Free();

};