#include "stdafx.h"
#include "Frame_Manager.h"
#include "Frame.h"

IMPLEMENT_SINGLETON(CFrame_Manager)

CFrame_Manager::CFrame_Manager()
{

}

CFrame_Manager::~CFrame_Manager()
{
}

HRESULT CFrame_Manager::Ready_Frames(const wchar_t * pTag, const float & fLimitCall)
{
	if (Find_Frame(pTag))
		return E_FAIL;

	CFrame* pFrame = CFrame::Create(fLimitCall);

	if (nullptr == pFrame)
		return E_FAIL;

	m_mapFrames.emplace(pTag, pFrame);

	return S_OK;
}

bool CFrame_Manager::Permit_Call(const wchar_t * pTag, const GameTimer & gt)
{
	CFrame* pFinder = Find_Frame(pTag);

	if (nullptr == pFinder)
		return false;

	return pFinder->Permit_Call(gt);
}

CFrame * CFrame_Manager::Find_Frame(const wchar_t * pTag)
{
	auto& finder = m_mapFrames.find(pTag);

	if (finder == m_mapFrames.end())
		return nullptr;

	return finder->second;
}

void CFrame_Manager::Free()
{
	for_each(m_mapFrames.begin(), m_mapFrames.end(), CRelease_Pair());
	m_mapFrames.clear();
}
