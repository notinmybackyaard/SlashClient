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

CComponent * CComponent_Manager::Clone_Component(wchar_t* pTag)
{
	CComponent* pComponent = Find_Component(pTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent->Clone();
}

HRESULT CComponent_Manager::Ready_Component(wchar_t* pTag, CComponent* pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (Find_Component(pTag))
		return E_FAIL;

	m_mapComponent.emplace(pTag, pComponent);

	return S_OK;
}

CComponent * CComponent_Manager::Find_Component(wchar_t* pTag)
{
	auto& iter = m_mapComponent.find(pTag);

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for_each(m_mapComponent.begin(), m_mapComponent.end(), CRelease_Pair());
	m_mapComponent.clear();
}
