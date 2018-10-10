#include "stdafx.h"
#include "SkillEffect.h"
#include "Effect.h"
#include "Transform.h"

CSkillEffect::CSkillEffect(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name)
	: CGameObject(d3dDevice, srv, srvSize)
	, m_strName(name)
{

}
CSkillEffect::CSkillEffect(CSkillEffect& other)
	: CGameObject(other.m_d3dDevice, other.mSrvDescriptorHeap, other.mCbvSrvDescriptorSize)
	, m_strName(other.m_strName)
	, m_EffectList(other.m_EffectList)
{
	for (auto elem : other.m_EffectList)
	{
		EFFECT_INFO info;
		info = elem->Get_EffectInfo();
		CEffect* effect = CEffect::Create(other.m_d3dDevice, other.mSrvDescriptorHeap, other.mCbvSrvDescriptorSize, info);
		//CEffect* effect = CEffect::Create(*elem);
		m_EffectList.push_back(effect);
	}
}

CSkillEffect::~CSkillEffect()
{
}

HRESULT CSkillEffect::Initialize()
{
	m_pTransCom = CTransform::Create(this);
	return S_OK;
}

bool CSkillEffect::Update(const GameTimer & gt)
{
	if (!m_IsEnable)
	{
		for (auto& elem : m_EffectList)
		{
			elem->Set_Enable(false);
		}
		return false;
	}


	if (!m_isPlay)
		return true;


	bool allEnable = true;

	vector<bool> vecEnable(m_EffectList.size());

	for (auto& elem : vecEnable)
	{
		elem = true;
	}

	int i = 0;
	for (auto& elem : m_EffectList)
	{
		if (elem->IsEnable() == true)
		{
			if (m_pParent != nullptr)
			{
				Set_ParentMatrix(&m_pParent->GetWorld());
			}
			elem->Update(gt);
			vecEnable[i] = true;
			//allEnable = true;
		}
		else
			vecEnable[i] = false;
		++i;
		/*else if (!elem->IsEnable() && allEnable)
		{
		allEnable = false;
		}*/

		/*if (elem->Update(gt) == false)
		m_EffectList.remove(elem);*/
	}
	for (auto& elem : vecEnable)
	{
		if (elem == true)
		{
			/*if (m_isContinue)
			{
				SetPlay(true);

			}*/
			//else
			{
				allEnable = true;
				m_IsEnable = true;

			}
			break;
		}

		else
		{
			m_IsEnable = false;
			allEnable = false;
		}

	}
	return m_IsEnable;
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
	m_IsEnable = _isPlay;

	for (auto& elem : m_EffectList)
		elem->SetPlay(_isPlay);
}

void CSkillEffect::Set_Enable(bool isEnable)
{
	m_IsEnable = isEnable;
	for (auto& elem : m_EffectList)
		elem->Set_Enable(isEnable);
}

void CSkillEffect::Set_ParentMatrix(XMFLOAT4X4* f4x4Parent)
{
	if (f4x4Parent == nullptr)
		return;

	XMFLOAT3 pos;
	memcpy(&pos, &f4x4Parent->_41, sizeof(float) * 3);
	//XMFLOAT4X4 mat = Matrix4x4::Identity();


	//mat._41 = f4x4Parent->_41;
	//mat._42 = f4x4Parent->_42;
	//mat._43 = f4x4Parent->_43;

	m_pTransCom->Translation(pos);
	m_pTransCom->Update_Component();
	//m_pTransCom->SetParentMatrix(&mat);
	//cout << "EffectSkill : " << m_pTransCom->GetPosition().x << ", " << m_pTransCom->GetPosition().y << ", " << m_pTransCom->GetPosition().z << endl;

	for (auto& elem : m_EffectList)
	{
		elem->GetTransform()->SetParentMatrix(&m_pTransCom->GetWorld());
	}
}

void CSkillEffect::Set_RotParentMatirx(XMFLOAT4X4 * f4x4Parent, float fRot)
{
	if (f4x4Parent == nullptr)
		return;

	XMFLOAT3 pos;
	memcpy(&pos, &f4x4Parent->_41, sizeof(float) * 3);

	m_pTransCom->Translation(pos);
	m_pTransCom->Rotation(0.f, fRot, 0.f);
	m_pTransCom->Update_Component();

	for (auto& elem : m_EffectList)
	{
		elem->GetTransform()->SetParentMatrix(&m_pTransCom->GetWorld());
	}
}

void CSkillEffect::Set_Parent(CGameObject* pObj)
{
	if (pObj == nullptr)
		return;

	m_pParent = pObj;

	Set_ParentMatrix(&pObj->GetWorld());
	//XMFLOAT4X4 world = pObj->GetTransform()
	/*for (auto& elem : m_EffectList)
	{
		elem->GetTransform()->SetParentMatrix(&(pObj->GetTransform()->GetWorld()));
	}*/
}

void CSkillEffect::Set_IsCon(bool iscon)
{
	m_isContinue = iscon;
	for (auto& elem : m_EffectList)
	{
		elem->SetIsCon(iscon);
	}
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

CSkillEffect * CSkillEffect::Create(CSkillEffect & other)
{
	CSkillEffect* pInstance = new CSkillEffect(other);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CSkillEffect Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillEffect::Free()
{
	for (auto elem : m_EffectList)
	{
		Safe_Release(elem);
	}
	m_EffectList.clear();
	CGameObject::Free();
}
