#include "stdafx.h"
#include "EffectScene.h"
#include "Management.h"
#include "Renderer.h"
#include "Layer.h"
#include "Skill.h"
#include "Terrain.h"
#include "Transform.h"
#include "Sound.h"
#include "Skill_Billboard.h"
#include "InputDevice.h"

CEffectScene::CEffectScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize)
	: CScene(d3dDevice, srv, srvSize)
{

}

CEffectScene::~CEffectScene()
{
}

HRESULT CEffectScene::Initialize()
{
	//CManagement::GetInstance()->GetSound()->PlayBGM(L"Assets/Sound", L"village");
	CGameObject* pObject = nullptr;
	/*CGameObject* pObject = CSkill::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Skill", pObject);
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, pObject);
	pObject->GetTransform()->Translation(30, 10, 0);

	pObject = CSkill::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Skill", pObject);
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, pObject);
	pObject->GetTransform()->Translation(10.f, 30.f, 0.f);
	pObject->GetTransform()->Scaling(10.f, 10.f, 1.f);*/


	pObject = Terrain::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Skill", pObject);
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);

	pObject = CSkill_Billboard::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, "Aura0");
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Skill", pObject);
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, pObject);
	return S_OK;
}

bool CEffectScene::Update(const GameTimer & gt)
{
	CScene::Update(gt);

	return true;
}

void CEffectScene::Render(ID3D12GraphicsCommandList * cmdList)
{
}

CEffectScene * CEffectScene::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>>& srv, UINT srvSize)
{
	CEffectScene* pInstance = new CEffectScene(d3dDevice, srv, srvSize);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CEffectScene Creatd Failed");
	}

	return pInstance;
}

void CEffectScene::Free()
{
	CScene::Free();
}
