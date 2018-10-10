#include "stdafx.h"
#include "Renderer.h"
#include "Define.h"
#include "GameObject.h"

CRenderer::CRenderer(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv)
	: mSrvDescriptorHeap(srv)
{
}

CRenderer::~CRenderer()
{
}

HRESULT CRenderer::Add_RenderGroup(RenderType type, CGameObject * pObj)
{
	m_vecObject[type].push_back(pObj);

	pObj->AddRef();

	return S_OK;
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}


void CRenderer::Update_Component(const GameTimer & gt)
{
}

void CRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap[HEAP_DEFAULT].Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	Render_ForWard(cmdList);

	Render_Priority(cmdList);

	//Render_Alpha(cmdList);


	ID3D12DescriptorHeap* descriptorHeaps1[] = { mSrvDescriptorHeap[HEAP_INSTANCING].Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps1), descriptorHeaps1);

	Render_Instancing(cmdList);

	Render_AlphaInstancing(cmdList);

	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	Render_Alpha(cmdList);
	Render_UI(cmdList);
	Clear_Renderer();
}

void CRenderer::Render_Priority(ID3D12GraphicsCommandList* cmdList)
{
	// ½¦ÀÌ´õ ¼³Á¤
	mCommandList->SetPipelineState(mPSOs["sky"].Get());
	for (auto& elem : m_vecObject[RENDER_PRIORITY])
		elem->Render(cmdList);
}

void CRenderer::Render_ForWard(ID3D12GraphicsCommandList* cmdList)
{
	mCommandList->SetPipelineState(mPSOs["Opaque"].Get());
	for (auto& elem : m_vecObject[RENDER_NONALPHA_FORWARD])
		elem->Render(cmdList);
}

void CRenderer::Render_Alpha(ID3D12GraphicsCommandList * cmdList)
{
	// > RENDER_ALPHA_SPRITE
	mCommandList->SetPipelineState(mPSOs["alphaBelnd"].Get());
	for (auto & elem : m_vecObject[RENDER_ALPHA_SPRITE])
	{
		elem->Render(cmdList);
		//mCommandList->ClearState(mPSOs["treeSprites"].Get());
	}
	// > 
	mCommandList->SetPipelineState(mPSOs["alphaBelnd_Object"].Get());
	for (auto & elem : m_vecObject[RENDER_ALPHA_DEFAULT])
	{
		elem->Render(cmdList);
		//mCommandList->ClearState(mPSOs["treeSprites"].Get());
	}

	// > 
}

void CRenderer::Render_Instancing(ID3D12GraphicsCommandList* cmdList)
{
	mCommandList->SetPipelineState(mPSOs["InstancingOpaque"].Get());
	for (auto& elem : m_vecObject[RENDER_NONALPHA_INSTANCING])
		elem->Render(cmdList);
}

void CRenderer::Render_AlphaInstancing(ID3D12GraphicsCommandList * cmdList)
{
	mCommandList->SetPipelineState(mPSOs["alphaTested_Inst"].Get());
	for (auto& elem : m_vecObject[RENDER_ALPHA_INST])
		elem->Render(cmdList);

	mCommandList->SetPipelineState(mPSOs["InstancingUI"].Get());
	for (auto& elem : m_vecObject[RENDER_UI_INST])
	{
		elem->Render(cmdList);
	}
}

void CRenderer::Render_UI(ID3D12GraphicsCommandList * cmdList)
{
	mCommandList->SetPipelineState(mPSOs["UI"].Get());
	for (auto& elem : m_vecObject[RENDER_UI])
		elem->Render(cmdList);

	mCommandList->SetPipelineState(mPSOs["UIChange"].Get());
	for (auto& elem : m_vecObject[RENDER_UICHANGE])
		elem->Render(cmdList);

}

void CRenderer::Clear_Renderer()
{
	for (int i = 0; i < RENDER_END; ++i)
	{
		size_t iSize = m_vecObject[i].size();
		for (int j = 0; j < iSize; ++j)
			Safe_Release(m_vecObject[i][j]);
		m_vecObject[i].clear();
	}
}

CComponent * CRenderer::Clone(void)
{
	AddRef();
	return this;
}

CRenderer * CRenderer::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv)
{
	CRenderer* pInstance = new CRenderer(d3dDevice, srv);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CRenderer Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderer::Free()
{
	for (int i = 0; i < RENDER_END; ++i)
	{
		for (auto& elem : m_vecObject[i])
			Safe_Release(elem);
		m_vecObject[i].clear();
	}
}
