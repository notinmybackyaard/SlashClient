#include "stdafx.h"
#include "Component_Manager.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

CComponent_Manager::~CComponent_Manager()
{
}

CComponent * CComponent_Manager::Clone_Component(wchar_t* pTag, COMTYPE eType)
{
	CComponent* pComponent = Find_Component(pTag, eType);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent->Clone();
}

CComponent * CComponent_Manager::Clone_Component(wchar_t * pTag)
{
	CComponent* pComponent;
	for (int i = 0; i < COM_END; ++i)
	{
		pComponent = Find_Component(pTag, COMTYPE(i));
		if (pComponent)
			return pComponent->Clone();
	}
	return nullptr;
}

HRESULT CComponent_Manager::Ready_Component(wchar_t* pTag, CComponent* pComponent, COMTYPE eType)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (Find_Component(pTag, eType))
		return E_FAIL;

	m_mapComponent[eType].emplace(pTag, pComponent);

	return S_OK;
}

CComponent * CComponent_Manager::Find_Component(wchar_t* pTag, COMTYPE eType)
{
	auto& iter = m_mapComponent[eType].find(pTag);

	if (iter == m_mapComponent[eType].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (int i = 0; i < COM_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CRelease_Pair());
		m_mapComponent[i].clear();
	}
}
