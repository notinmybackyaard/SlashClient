#include "stdafx.h"
#include "GameTimer_Manager.h"
#include "GameTimer.h"

IMPLEMENT_SINGLETON(CGameTimer_Manager);

CGameTimer_Manager::CGameTimer_Manager()
{
}

CGameTimer_Manager::~CGameTimer_Manager()
{
}

GameTimer* CGameTimer_Manager::Get_GameTimer(const wchar_t * pTag)
{
	GameTimer* pTimer = Find_GameTimer(pTag);

	return pTimer;
}

HRESULT CGameTimer_Manager::Ready_Timers(const wchar_t * pTag)
{
	if (Find_GameTimer(pTag))
		return E_FAIL;

	GameTimer* pTimer = new GameTimer;

	if (nullptr == pTimer)
		return E_FAIL;
	
	m_mapTimers.emplace(pTag, pTimer);

	return E_NOTIMPL;
}

void CGameTimer_Manager::Compute_TimeDelta(const wchar_t * pTag)
{
	GameTimer* pTimer = Find_GameTimer(pTag);

	if (nullptr == pTimer)
		return;
	
	pTimer->Tick();
}

GameTimer* CGameTimer_Manager::Find_GameTimer(const wchar_t * pTag)
{
	auto& finder = m_mapTimers.find(pTag);

	if (finder == m_mapTimers.end())
		return nullptr;

	return finder->second;
}

void CGameTimer_Manager::Free()
{
	for_each(m_mapTimers.begin(), m_mapTimers.end(), CRelease_Pair());
	m_mapTimers.clear();
}
