#include "stdafx.h"
#include "Layer.h"
#include "GameObject.h"
#include "Define.h"

CLayer::CLayer()
{

}

CLayer::~CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

bool CLayer::Update(const GameTimer & gt)
{
	size_t iSize = m_vecObject.size();
	
	for (size_t i = 0; i < iSize; ++i)
	{
		if (false == m_vecObject[i]->IsEnable())
			continue;
		if (!(m_vecObject[i]->Update(gt)))
		{
			Erase_Vector_Element(m_vecObject, i--);
			--iSize;
		}
	}

	return true;
}

void CLayer::Render(ID3D12GraphicsCommandList * cmdList)
{
}

HRESULT CLayer::Add_Object(CGameObject * pObj)
{
	if (nullptr == pObj)
		return E_FAIL;

	m_vecObject.push_back(pObj);

	return S_OK;
}

CLayer * CLayer::Create()
{
	CLayer* pInstance = new CLayer;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CLayer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& elem : m_vecObject)
		Safe_Release(elem);
	m_vecObject.clear();
}
