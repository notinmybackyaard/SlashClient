#include "stdafx.h"
#include "Frame.h"
#include "GameTimer.h"

CFrame::CFrame()
	: m_fCallPerSec(0.f)
	, m_fTimeDeltaAcc(0.f)
{

}

CFrame::~CFrame()
{
}

HRESULT CFrame::Ready_Frame(const float & fCallCnt)
{
	m_fCallPerSec = 1.f / fCallCnt;

	return S_OK;
}

bool CFrame::Permit_Call(const GameTimer & gt)
{
	m_fTimeDeltaAcc += gt.DeltaTime();

	if (m_fTimeDeltaAcc >= m_fCallPerSec)
	{
		m_fTimeDeltaAcc = 0.f;
		return true;
	}

	return false;
}

CFrame * CFrame::Create(const float & fCallCnt)
{
	CFrame* pInstance = new CFrame;
	
	if (FAILED(pInstance->Ready_Frame(fCallCnt)))
	{
		MSG_BOX(L"CFrame Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrame::Free()
{
}