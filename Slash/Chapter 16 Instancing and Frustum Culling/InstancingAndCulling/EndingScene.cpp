#include "stdafx.h"
#include "EndingScene.h"
#include "Management.h"
#include "Renderer.h"
#include "Layer.h"
#include "Terrain.h"
#include "Transform.h"
#include "Sound.h"
#include "InputDevice.h"
#include "ChangeUI.h"
#include "Skeleton.h"
#include "Texture_Manager.h"
#include "TestScene.h"
#include "StaticUI.h"
#include "HPBar.h"
#include "SelectScene.h"
#include "Terrain.h"
#include "SelectPlayer.h"

CEndingScene::CEndingScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize)
	: CScene(d3dDevice, srv, srvSize)
{

}

CEndingScene::~CEndingScene()
{
}

HRESULT CEndingScene::Initialize()
{
	XMFLOAT2 move;
	XMFLOAT2 scale;
	float size = 1.f;

	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("StartScene", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 0.f;
	scale.x = 1.0f;
	scale.y = 1.0f;
	size = 1.0f;
	CGameObject* pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Ending", tex->Num, true);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	Ready_GameObject(L"Layer_EndingSceneUI", pObject);


	tex = CTexture_Manager::GetInstance()->Find_Texture("TheEnd", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = -0.f;
	scale.x = 0.7f;
	scale.y = 0.4f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_TheEnd", tex->Num, true);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f), 1.5f);
	dynamic_cast<ChangeUI*>(pObject)->SetInitColor(XMFLOAT4(1.f, 1.f, 1.f, 0.2f));
	Ready_GameObject(L"Layer_EndingSceneUI", pObject);

	return S_OK;
}

bool CEndingScene::Update(const GameTimer & gt)
{
	CScene::Update(gt);

	return true;
}

void CEndingScene::Render(ID3D12GraphicsCommandList * cmdList)
{
}

CEndingScene * CEndingScene::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>>& srv, UINT srvSize)
{
	CEndingScene* pInstance = new CEndingScene(d3dDevice, srv, srvSize);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CEndingScene Creatd Failed");
	}

	return pInstance;
}

void CEndingScene::Free()
{
	CScene::Free();
}
