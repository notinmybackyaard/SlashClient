#pragma once

#include "Base.h"
#include "Define.h"
#include "Renderer.h"

class CGameObject;
class GameTimer;
class Camera;

class CObjectManager
	: public CBase
{
	DECLARE_SINGLETON(CObjectManager)

public:
	enum OBJTYPE{OBJ_DEFAULT, OBJ_PREVIEW, OBJ_EFFECT, OBJ_END};

private:
	explicit CObjectManager();
	virtual ~CObjectManager();

public:
	vector<ComPtr<ID3D12DescriptorHeap>>		GetSrvDescriptorHeap() { return m_pRenderer->GetSrvDescriptorHeap(); }
	UINT&										GetCbvSrvDescriptorSize() { return m_pRenderer->GetCbvSrvDescriptorSize(); }
	FrameResource*								GetCurFrameResource() { return mCurrFrameResource; }
	Microsoft::WRL::ComPtr<ID3D12Device>		GetDevice() { return m_d3dDevice; }
	CRenderer*									GetRenderer() { return m_pRenderer; }

	vector<CGameObject*>						Get_Objects(OBJTYPE eType) { return m_vecObject[eType]; }

	Camera*										Get_MainCam() { return m_pMainCam; }

	void			Set_MainCam(Camera* pCamera) { if (pCamera == nullptr) return; m_pMainCam = pCamera; }
	void			Clear_Object();

	CGameObject*	Find_Object(unsigned int iIdx, OBJTYPE eType = OBJ_DEFAULT);
	void			Delete_Object(unsigned int iIdx, OBJTYPE eType = OBJ_DEFAULT);
	void			Delete_All_Object(OBJTYPE eType = OBJ_DEFAULT);

	HRESULT			Add_Object(CGameObject* pObj, OBJTYPE eType = OBJ_DEFAULT);

public:
	HRESULT		Init_ObjMgr(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, CRenderer* pRenderer);
	int			Update_ObjMgr(const GameTimer& gt, const FrameResource* pCurRsc);
	void		Render_ObjMgr(ID3D12GraphicsCommandList* cmdList);

	
private:
	CRenderer*						m_pRenderer;
	vector<CGameObject*>			m_vecObject[OBJ_END];
	Camera*							m_pMainCam = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
	UINT mCbvSrvDescriptorSize = 0;

	FrameResource* mCurrFrameResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device>			m_d3dDevice;

private:
	virtual void Free();
};