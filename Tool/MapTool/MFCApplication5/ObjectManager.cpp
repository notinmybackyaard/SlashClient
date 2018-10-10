#include "stdafx.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CObjectManager);

CObjectManager::CObjectManager()
	: m_pRenderer(nullptr)
{

}

CObjectManager::~CObjectManager()
{
}

void CObjectManager::Clear_Object()
{
	for (int j = 0; j < OBJ_END; ++j)
	{
		size_t iSize = m_vecObject[j].size();

		for (size_t i = 0; i < iSize; ++i)
			Safe_Release(m_vecObject[j][i]);
		m_vecObject[j].clear();
	}

	m_pRenderer->Clear_Renderer();
}

CGameObject * CObjectManager::Find_Object(unsigned int iIdx, OBJTYPE eType)
{
	if(iIdx > m_vecObject[eType].size() - 1)
		return nullptr;

	return m_vecObject[eType][iIdx];
}

void CObjectManager::Delete_Object(unsigned int iIdx, OBJTYPE eType)
{
	size_t iSize = m_vecObject[eType].size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecObject[eType][i]->GetMyID() == iIdx)
		{
			Erase_Vector_Element(m_vecObject[eType], i);
			return;
		}
	}
}

void CObjectManager::Delete_All_Object(OBJTYPE eType)
{
	size_t iSize = m_vecObject[eType].size();

	for (size_t i = 0; i < iSize; ++i)
		Safe_Release(m_vecObject[eType][i]);
	m_vecObject[eType].clear();
}

HRESULT CObjectManager::Add_Object(CGameObject * pObj, OBJTYPE eType)
{
	if (pObj == nullptr)
		return E_FAIL;

	m_vecObject[eType].push_back(pObj);

	return S_OK;
}

HRESULT CObjectManager::Init_ObjMgr(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, CRenderer* pRenderer)
{
	m_d3dDevice = d3dDevice;
	m_pRenderer = pRenderer;
	return S_OK;
}

int CObjectManager::Update_ObjMgr(const GameTimer & gt, const FrameResource* pCurRsc)
{
	mCurrFrameResource = const_cast<FrameResource*>(pCurRsc);
	for (int j = 0; j < OBJ_END; ++j)
	{
		size_t iSize = m_vecObject[j].size();

		for (size_t i = 0; i < iSize; ++i)
			m_vecObject[j][i]->Update(gt);
	}
	return 0;
}

void CObjectManager::Render_ObjMgr(ID3D12GraphicsCommandList* cmdList)
{
	m_pRenderer->Render(cmdList);
}

void CObjectManager::Free()
{
	for (int j = 0; j < OBJ_END; ++j)
	{
		size_t iSize = m_vecObject[j].size();
		for (size_t i = 0; i < iSize; ++i)
			Safe_Release(m_vecObject[j][i]);
		m_vecObject[j].clear();
	}

}
