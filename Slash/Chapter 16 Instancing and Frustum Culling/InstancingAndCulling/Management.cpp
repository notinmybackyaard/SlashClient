#include "stdafx.h"
#include "Management.h"
#include "Renderer.h"
#include "NumUI_Inst.h"
#include "Player.h"
#include "ChangeUI.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pRenderer(nullptr)
	, m_pCurScene(nullptr)
	, m_pNumUI(nullptr)
{

}

CManagement::~CManagement()
{
}

CLayer* CManagement::Get_Layer(wchar_t* pLayerTag)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pCurScene->Get_Layer(pLayerTag);
}

CGameObject * CManagement::Find_Object(wchar_t * LayerTag, unsigned int iIdx)
{
	return m_pCurScene->Find_Object(LayerTag, iIdx);
}

HRESULT CManagement::Add_NumUI(int iNum, XMFLOAT3 f3Pos, bool isCritical)
{
	m_pNumUI->Add(iNum, f3Pos, isCritical);

	return S_OK;
}

void CManagement::SetExp(CGameObject * pObj, float _exp)
{
	if (pObj == nullptr)
		return;
	pObj->SetExp(_exp);
}

void CManagement::AddExp(CGameObject * pObj, float _exp)
{
	if (pObj == nullptr || !dynamic_cast<Player*>(pObj))
		return;
	dynamic_cast<Player*>(pObj)->AddExp(_exp);
}

void CManagement::SetLevel(CGameObject* pObj, int iLv)
{
	if (pObj == nullptr || !dynamic_cast<Player*>(pObj))
		return;
	dynamic_cast<Player*>(pObj)->SetLevel(iLv);
}

void CManagement::PlayLevelUP()
{
	CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"LevelUp");


	size_t iSize = m_pLevelUP.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		m_pLevelUP[i]->SetPlay(true);
	}
}

void CManagement::SetLevelUPUI(vector<ChangeUI*> vec)
{
	m_pLevelUP = vec;
}

void CManagement::PlaySkillUI(int inum)
{
	if(inum < 0 && inum > m_pSkillReady.size() - 1)
	m_pSkillReady[inum]->SetPlay(true);
	/*size_t iSize = m_pSkillReady.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		m_pSkillReady[i]->SetPlay(true);
	}*/
}

void CManagement::SetSkillUPUI(vector<ChangeUI*> vec)
{
	m_pSkillReady = vec;
}


HRESULT CManagement::Init_Management(CRenderer* pRenderer, NumUI_Inst* pNumUI)
{
	if (pRenderer == nullptr || pNumUI == nullptr)
		return E_FAIL;

	m_pRenderer = pRenderer;
	m_pNumUI = pNumUI;

	return S_OK;
}

bool CManagement::Update(const GameTimer & gt, const FrameResource* pCruRrc)
{
	mCurrFrameResource = const_cast<FrameResource*>(pCruRrc);

	if (false == m_pCurScene->Update(gt))
		return false;

	m_pNumUI->Update(gt);

	size_t iSize = m_pLevelUP.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		m_pLevelUP[i]->Update(gt);
	}
	iSize = m_pSkillReady.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		m_pSkillReady[i]->Update(gt);
	}
	return true;

}

void CManagement::Render(ID3D12GraphicsCommandList * cmdList)
{
	m_pRenderer->Render(cmdList);
}

HRESULT CManagement::Change_Scene(CScene* pScene)
{
	if (nullptr == pScene)
		return E_FAIL;

	Safe_Release(m_pCurScene);

	m_pCurScene = pScene;

	m_pCurScene->Get_MainCam()->Set_Object(CManagement::GetInstance()->Get_CurScene()->Find_Object(L"Layer_Player", 0));

	return S_OK;
}

HRESULT CManagement::Set_Sound(CSound * pSound)
{
	if (pSound == nullptr)
		return E_FAIL;
	m_pSound = pSound;

	return S_OK;
}

void CManagement::Free()
{
	Safe_Release(m_pCurScene);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pSound);
}