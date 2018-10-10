#include "stdafx.h"
#include "SkillEffect.h"
#include "Effect.h"

CSkillEffect::CSkillEffect(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name)
	: CGameObject(d3dDevice, srv, srvSize)
	, m_strName(name)
{

}

CSkillEffect::~CSkillEffect()
{
}

HRESULT CSkillEffect::Initialize()
{
	return S_OK;
}

bool CSkillEffect::Update(const GameTimer & gt)
{
	if (!m_IsEnable)
		return true;


	if (!m_isPlay)
		return true;

	for (auto& elem : m_EffectList)
	{
		elem->Update(gt);
		/*if (elem->Update(gt) == false)
			m_EffectList.remove(elem);*/
	}
	return true;
}

void CSkillEffect::Render(ID3D12GraphicsCommandList * cmdList)
{
	/*for (auto& elem : m_EffectList)
	{
		elem->Render(cmdList);
	}*/
}

void CSkillEffect::SetPlay(bool _isPlay)
{
	m_isPlay = _isPlay;

	for (auto& elem : m_EffectList)
		elem->SetPlay(_isPlay);
}

void CSkillEffect::Set_Enable(bool isEnable)
{
	m_IsEnable = isEnable;
	for (auto& elem : m_EffectList)
		elem->Set_Enable(isEnable);
}

CSkillEffect * CSkillEffect::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name)
{
	CSkillEffect* pInstance = new CSkillEffect(d3dDevice, srv, srvSize, name);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CSkillEffect Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillEffect::Free()
{
	CGameObject::Free();
}
