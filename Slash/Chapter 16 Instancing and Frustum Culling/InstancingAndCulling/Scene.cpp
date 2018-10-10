#include "stdafx.h"
#include "Scene.h"
#include "Layer.h"
#include "Define.h"

CScene::CScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize)
	: m_d3dDevice(d3dDevice)
	, mSrvDescriptorHeap(srv)
	, mCbvSrvDescriptorSize(srvSize)
	, m_pMainCam(nullptr)
{
}

CScene::~CScene()
{
}

CLayer* CScene::Get_Layer(wchar_t * pLayerTag)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	auto finder = m_mapLayer.find(pLayerTag);
	if (finder == m_mapLayer.end())
		return nullptr;

	return finder->second;
}

CGameObject * CScene::Find_Object(wchar_t* LayerTag, unsigned int iIdx)
{
	CLayer* pLayer = Find_Layer(LayerTag);

	if (nullptr == pLayer)
		return nullptr;
	
	return pLayer->Get_Object(iIdx);
}

HRESULT CScene::Initialize()
{
	return S_OK;
}

bool CScene::Update(const GameTimer & gt)
{
	auto& iter = m_mapLayer.begin();

	for (; iter != m_mapLayer.end(); )
	{
		if (iter->second->IsEnable() == false) {
			++iter;
			continue;
		}
		if (iter->second->Update(gt) == false)
			iter = m_mapLayer.erase(iter);
		else
			++iter;
	}

	return true;
}

void CScene::Render(ID3D12GraphicsCommandList * cmdList)
{
}

HRESULT CScene::Add_Layer(wchar_t* pLayerTag, CLayer * pLayer)
{
	if (Find_Layer(pLayerTag) != nullptr)
		return E_FAIL;

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

CLayer * CScene::Find_Layer(wchar_t * pLayerTag)
{
	auto& finder = m_mapLayer.find(pLayerTag);

	if (finder == m_mapLayer.end())
		return nullptr;

	return finder->second;

}

HRESULT CScene::Ready_GameObject(wchar_t * pLayerTag, CGameObject * pObj)
{
	if (nullptr == pObj)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(pLayerTag);
	if (!pLayer)
	{
		if (FAILED(Ready_Layer(pLayerTag)))
			return E_FAIL;
		pLayer = Find_Layer(pLayerTag);
	}

	pLayer->Add_Object(pObj);

	return S_OK;
}

HRESULT CScene::Ready_Layer(wchar_t * pLayerTag)
{
	CLayer* pLayer = CLayer::Create();

	if (nullptr == pLayer)
		return E_FAIL;

	if (FAILED(Add_Layer(pLayerTag, pLayer)))
		return E_FAIL;

	return S_OK;
}

void CScene::Free()
{
	//Safe_Release(m_pMainCam);
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CRelease_Pair());
	m_mapLayer.clear();
}
