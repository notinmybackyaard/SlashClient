// EffectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCApplication5.h"
#include "EffectTool.h"
#include "afxdialogex.h"

#include "View_0.h"
#include "GameObject.h"
#include "Skill_Billboard.h"
#include "ObjectManager.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Terrain.h"
#include "Effect.h"
#include "SkillEffect.h"

// CEffectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectTool, CDialog)

CEffectTool::CEffectTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EFFECTTOOL, pParent)
	, m_SPos(0.f, 0.f, 0.f)
	, m_EPos(0.f, 0.f, 0.f)
	, m_SSize(1.f, 1.f, 1.f)
	, m_ESize(1.f, 1.f, 1.f)
	, m_SRot(-90.f, 0.f, 0.f)
	, m_ERot(-90.f, 0.f, 0.f)
	, m_SColor(1.f, 1.f, 1.f, 1.f)
	, m_EColor(1.f, 1.f, 1.f, 1.f)
	, m_fLifeTime(0.f)
	, m_fStartTime(0.f)
	, m_tcurFrameInfo(XMFLOAT2(0.f, 0.f), XMFLOAT2(0.f, 0.f), XMFLOAT2(1.f, 1.f), 1.f, 1, true)
	, m_cstrEffectName(_T(""))
	, m_cstrSkillEffectName(_T(""))
{

}

CEffectTool::~CEffectTool()
{
}

void CEffectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEX, m_TexListBox);
	DDX_Text(pDX, IDC_EDIT_S_POS_X, m_SPos.x);
	DDX_Text(pDX, IDC_EDIT_S_POS_Y, m_SPos.y);
	DDX_Text(pDX, IDC_EDIT_S_POS_Z, m_SPos.z);

	DDX_Text(pDX, IDC_EDIT_E_POS_X, m_EPos.x);
	DDX_Text(pDX, IDC_EDIT_E_POS_Y, m_EPos.y);
	DDX_Text(pDX, IDC_EDIT_E_POS_Z, m_EPos.z);

	DDX_Text(pDX, IDC_EDIT_S_SIZE_X, m_SSize.x);
	DDX_Text(pDX, IDC_EDIT_S_SIZE_Y, m_SSize.y);
	DDX_Text(pDX, IDC_EDIT_S_SIZE_Z, m_SSize.z);

	DDX_Text(pDX, IDC_EDIT_E_SIZE_X, m_ESize.x);
	DDX_Text(pDX, IDC_EDIT_E_SIZE_Y, m_ESize.y);
	DDX_Text(pDX, IDC_EDIT_E_SIZE_Z, m_ESize.z);

	DDX_Text(pDX, IDC_EDIT_S_ROT_X, m_SRot.x);
	DDX_Text(pDX, IDC_EDIT_S_ROT_Y, m_SRot.y);
	DDX_Text(pDX, IDC_EDIT_S_ROT_Z, m_SRot.z);

	DDX_Text(pDX, IDC_EDIT_E_ROT_X, m_ERot.x);
	DDX_Text(pDX, IDC_EDIT_E_ROT_Y, m_ERot.y);
	DDX_Text(pDX, IDC_EDIT_E_ROT_Z, m_ERot.z);

	DDX_Text(pDX, IDC_EDIT_S_COLOR_R, m_SColor.x);
	DDX_Text(pDX, IDC_EDIT_S_COLOR_G, m_SColor.y);
	DDX_Text(pDX, IDC_EDIT_S_COLOR_B, m_SColor.z);
	DDX_Text(pDX, IDC_EDIT_S_ALPHA, m_SColor.w);

	DDX_Text(pDX, IDC_EDIT_E_COLOR_R, m_EColor.x);
	DDX_Text(pDX, IDC_EDIT_E_COLOR_G, m_EColor.y);
	DDX_Text(pDX, IDC_EDIT_E_COLOR_B, m_EColor.z);
	DDX_Text(pDX, IDC_EDIT_E_ALPHA, m_EColor.w);

	DDX_Text(pDX, IDC_EDIT_LIFETIME, m_fLifeTime);
	DDX_Text(pDX, IDC_EDIT_STARTTIME, m_fStartTime);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_EffectListBox);
	DDX_Control(pDX, IDC_CHECK_EFFECT_BILLBOARD, m_ButtonIsBillboard);


	DDX_Text(pDX, IDC_EDIT_EFFECT_FRAME_MAX_X, m_tcurFrameInfo.f2maxFrame.x);
	DDX_Text(pDX, IDC_EDIT_EFFECT_FRAME_MAX_Y, m_tcurFrameInfo.f2maxFrame.y);
	DDX_Text(pDX, IDC_EDIT_EFFECT_FRAME_SIZE_X, m_tcurFrameInfo.f2FrameSize.x);
	DDX_Text(pDX, IDC_EDIT_EFFECT_FRAME_SIZE_Y, m_tcurFrameInfo.f2FrameSize.y);
	DDX_Text(pDX, IDC_EDIT_EFFECT_FRAME_SPEED, m_tcurFrameInfo.fSpeed);
	DDX_Control(pDX, IDC_CHECK_EFFECT_FRAME, m_ButtonIsFrame);
	DDX_Control(pDX, IDC_CHECK_END_FRAME, m_ButtonFrameCnt);

	DDX_Text(pDX, IDC_EDIT_FRAME_END_CNT, m_tcurFrameInfo.iPlayCnt);
	DDX_Text(pDX, IDC_EDIT_EFFECT_NAME, m_cstrEffectName);
	DDX_Control(pDX, IDC_LIST_SKILL_EFFECT, m_SkillEffectListBox);
	DDX_Control(pDX, IDC_LIST_COMPLETE_SKILL_EFFECT, m_ComSkillEffectListBox);
	DDX_Text(pDX, IDC_EDIT_SKILLEFFECT_NAME, m_cstrSkillEffectName);
	DDX_Control(pDX, IDC_LIST_MODEL, m_ModelListBox);
}


BEGIN_MESSAGE_MAP(CEffectTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_TEX, &CEffectTool::OnLbnSelchangeListTex)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EFFECT, &CEffectTool::OnBnClickedButtonAddEffect)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_EFFECT, &CEffectTool::OnBnClickedButtonApplyEffect)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_EFFECT, &CEffectTool::OnBnClickedButtonPlayEffect)
	ON_BN_CLICKED(IDC_BUTTON_STOP_EFFECT, &CEffectTool::OnBnClickedButtonStopEffect)
	ON_BN_CLICKED(IDC_CHECK_EFFECT_FRAME, &CEffectTool::OnBnClickedCheckEffectFrame)
	ON_BN_CLICKED(IDC_CHECK_END_FRAME, &CEffectTool::OnBnClickedCheckEndFrame)
	ON_LBN_SELCHANGE(IDC_LIST_EFFECT, &CEffectTool::OnLbnSelchangeListEffect)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE_SELECT, &CEffectTool::OnBnClickedButtonCancleSelect)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SKILL_EFFECT, &CEffectTool::OnBnClickedButtonAddSkillEffect)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_SKILLEFFECT, &CEffectTool::OnBnClickedButtonMakeSkilleffect)
	ON_LBN_SELCHANGE(IDC_LIST_COMPLETE_SKILL_EFFECT, &CEffectTool::OnLbnSelchangeListCompleteSkillEffect)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CEffectTool::OnBnClickedButtonSave)
	ON_LBN_SELCHANGE(IDC_LIST_MODEL, &CEffectTool::OnLbnSelchangeListModel)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, &CEffectTool::OnBnClickedButtonShow)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_ALL, &CEffectTool::OnBnClickedButtonPlayAll)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ALL, &CEffectTool::OnBnClickedButtonStopAll)
END_MESSAGE_MAP()


// CEffectTool 메시지 처리기입니다.


BOOL CEffectTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	RECT Rect = { 0, 0, 1300, 800 };
	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, FALSE);
	int width = Rect.right - Rect.left;
	int height = Rect.bottom - Rect.top;
	SetWindowPos(NULL, 100, 50, width, height, 0);

	// 맵뷰 크기 지정
	m_Context.m_pNewViewClass = RUNTIME_CLASS(CView_0);
	m_pView = (CView_0*)((CFrameWnd*)this)->CreateView(&m_Context);
	m_pView->OnInitialUpdate();
	m_pView->ShowWindow(SW_NORMAL);


	//CGameObject* pObject = CSkill_Billboard::Create(CObjectManager::GetInstance()->GetDevice(), CObjectManager::GetInstance()->GetSrvDescriptorHeap()[HEAP_DEFAULT], CObjectManager::GetInstance()->GetCbvSrvDescriptorSize(), "Aura0");
	//CObjectManager::GetInstance()->Add_Object(pObject);
	//CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);

	CGameObject* pObject = Terrain::Create(CObjectManager::GetInstance()->GetDevice(), CObjectManager::GetInstance()->GetSrvDescriptorHeap()[HEAP_DEFAULT], CObjectManager::GetInstance()->GetCbvSrvDescriptorSize());
	CObjectManager::GetInstance()->Add_Object(pObject);
	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);

	pObject = Terrain::Create(CObjectManager::GetInstance()->GetDevice(), CObjectManager::GetInstance()->GetSrvDescriptorHeap()[HEAP_DEFAULT], CObjectManager::GetInstance()->GetCbvSrvDescriptorSize());
	CObjectManager::GetInstance()->Add_Object(pObject);
	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);
	pObject->Rotate(0.f, 0.f, -90.f);
	

	// > Model List
	unordered_map<wstring, CComponent*> mapComponent = CComponent_Manager::GetInstance()->Get_ComponentMap(CComponent_Manager::COM_MESH);

	auto iter = mapComponent.begin();
	auto iter_end = mapComponent.end();
	for (iter; iter != iter_end; ++iter)
	{
		m_ModelListBox.AddString(iter->first.c_str());
	}

	// > Texture List 
	unordered_map<wstring, Texture*> mapTex = CTexture_Manager::GetInstance()->Get_TextureMap(HEAP_TEXTURE_EFFECT);
	auto Texiter = mapTex.begin();
	auto Texiter_end = mapTex.end();
	for (Texiter; Texiter != Texiter_end; ++Texiter)
	{
		m_TexListBox.AddString(Texiter->first.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CEffectTool::OnLbnSelchangeListTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	int iSel = m_TexListBox.GetCurSel();

	CString cstrTexName;
	m_TexListBox.GetText(iSel, cstrTexName);

	CT2CA pszConvertedAnsiString(cstrTexName);
	std::string strTex(pszConvertedAnsiString);

	if (m_pCurObject == nullptr)
		return;

	dynamic_cast<CEffect*>(m_pCurObject)->SetTexture(strTex);

	//if (m_IsReplace)
	//{
	//	EFFECT_INFO tInfo(m_SPos, m_EPos, m_SSize, m_ESize, m_SRot, m_ERot, m_SColor, m_EColor, m_fLifeTime, m_fStartTime, "", strTex, m_ButtonIsBillboard.GetCheck());
	//	m_pCurObject = CEffect::Create(CObjectManager::GetInstance()->GetDevice()
	//		, CObjectManager::GetInstance()->GetSrvDescriptorHeap()[HEAP_DEFAULT]
	//		, CObjectManager::GetInstance()->GetCbvSrvDescriptorSize()
	//		, tInfo);

	//	CObjectManager::GetInstance()->Add_Object(m_pCurObject);
	//	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, m_pCurObject);
	//	m_IsReplace = false;
	//}

	//else
	//{
	//	dynamic_cast<CEffect*>(m_pCurObject)->SetTexture(strTex);
	//}
	UpdateData(FALSE);
}


void CEffectTool::OnBnClickedButtonAddEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	int finder = m_EffectListBox.FindStringExact(0, m_cstrEffectName);

	// > 추가하려는 이펙트가 이미 존재함
	if (finder > 0)
	{
		string Texname = dynamic_cast<CEffect*>(m_vecEffect[finder])->Get_EffectInfo().strTexName;
		EFFECT_INFO tInfo(m_SPos, m_EPos, m_SSize, m_ESize, m_SRot, m_ERot, m_SColor, m_EColor, m_fLifeTime, m_fStartTime, "", Texname, m_ButtonIsBillboard.GetCheck());
		dynamic_cast<CEffect*>(m_vecEffect[finder])->Get_EffectInfo() = tInfo;

		if (m_ButtonIsFrame.GetCheck())
		{
			UV_FRAME_INFO tFrameInfo(m_tcurFrameInfo);
			tFrameInfo.isEndbyCnt = m_ButtonFrameCnt.GetCheck();
			dynamic_cast<CEffect*>(m_vecEffect[finder])->Get_FrameInfo() = tFrameInfo;
			dynamic_cast<CEffect*>(m_vecEffect[finder])->SetIsFrame(true);
		}
		else
		{
			dynamic_cast<CEffect*>(m_vecEffect[finder])->SetIsFrame(false);
		}
	}
	
	else
	{
		CT2CA pszConvertedAnsiString(m_cstrEffectName);
		dynamic_cast<CEffect*>(m_pCurObject)->Get_EffectInfo().strName = pszConvertedAnsiString;
		m_vecEffect.push_back(m_pCurObject);
		m_EffectListBox.AddString(m_cstrEffectName);
	}
	m_pCurObject->Set_Enable(false);
	//delete m_pCurObject;
	m_pCurObject = nullptr;
	m_IsReplace = true;

	UpdateData(FALSE);
}


void CEffectTool::OnBnClickedButtonApplyEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	string Texname = dynamic_cast<CEffect*>(m_pCurObject)->Get_EffectInfo().strTexName;
	EFFECT_INFO tInfo(m_SPos, m_EPos, m_SSize, m_ESize, m_SRot, m_ERot, m_SColor, m_EColor, m_fLifeTime, m_fStartTime, "", Texname, m_ButtonIsBillboard.GetCheck());
	dynamic_cast<CEffect*>(m_pCurObject)->Get_EffectInfo() = tInfo;

	if (m_ButtonIsFrame.GetCheck())
	{
		UV_FRAME_INFO tFrameInfo(m_tcurFrameInfo);
		tFrameInfo.isEndbyCnt = m_ButtonFrameCnt.GetCheck();
		dynamic_cast<CEffect*>(m_pCurObject)->Get_FrameInfo() = tFrameInfo;
		dynamic_cast<CEffect*>(m_pCurObject)->SetIsFrame(true);
	}
	else
	{
		dynamic_cast<CEffect*>(m_pCurObject)->SetIsFrame(false);
	}


	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedButtonPlayEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	if(dynamic_cast<CEffect*>(m_pCurObject))
		dynamic_cast<CEffect*>(m_pCurObject)->SetPlay(true);
	if (dynamic_cast<CSkillEffect*>(m_pCurObject))
		dynamic_cast<CSkillEffect*>(m_pCurObject)->SetPlay(true);
	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedButtonStopEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	if (dynamic_cast<CEffect*>(m_pCurObject))
		dynamic_cast<CEffect*>(m_pCurObject)->SetPlay(false);
	if (dynamic_cast<CSkillEffect*>(m_pCurObject))
		dynamic_cast<CSkillEffect*>(m_pCurObject)->SetPlay(false);
	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedCheckEffectFrame()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	if (m_ButtonIsFrame.GetCheck())
	{
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_MAX_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_MAX_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_SIZE_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_SIZE_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_SPEED)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_END_FRAME)->EnableWindow(TRUE);
	}
	else if (!m_ButtonIsFrame.GetCheck())
	{
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_MAX_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_MAX_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_SIZE_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_SIZE_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_EFFECT_FRAME_SPEED)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_END_FRAME)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedCheckEndFrame()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	if (m_ButtonFrameCnt.GetCheck())
	{
		GetDlgItem(IDC_EDIT_FRAME_END_CNT)->EnableWindow(TRUE);
	}
	else if (!m_ButtonFrameCnt.GetCheck())
	{
		GetDlgItem(IDC_EDIT_FRAME_END_CNT)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CEffectTool::OnLbnSelchangeListEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	if (m_pCurObject)
	{
		return;
		m_pCurObject->Set_Enable(false);
		m_pCurObject = nullptr;

	}

	int iSel = m_EffectListBox.GetCurSel();
	if (iSel > m_vecEffect.size())
		return;

	EFFECT_INFO info = dynamic_cast<CEffect*>(m_vecEffect[iSel])->Get_EffectInfo();
	m_SPos = info.S_Pos;
	m_EPos = info.E_Pos;
	m_SSize = info.S_Size;
	m_ESize = info.E_Size;
	m_SRot = info.S_Rot;
	m_ERot = info.E_Rot;
	m_SColor = info.S_Color;
	m_EColor = info.E_Color;
	m_fStartTime = info.StartTime;
	m_fLifeTime = info.LifeTime;
	int tex = m_TexListBox.FindStringExact(0, CString::CStringT(CA2CT(info.strTexName.c_str())));
	m_TexListBox.SetCurSel(tex);

//	if (m_pCurObject)
//		delete m_pCurObject;
	m_pCurObject = m_vecEffect[iSel];
	m_pCurObject->Set_Enable(true);
	m_IsReplace = false;
	
	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedButtonCancleSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	if (m_pCurObject)
	{
		m_IsReplace = true;
		m_pCurObject->Set_Enable(false);
		m_pCurObject = nullptr;
	}

	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedButtonAddSkillEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	int iSel = m_EffectListBox.GetCurSel();
	if (iSel > m_vecEffect.size())
		return;

	CEffect* pEffect = dynamic_cast<CEffect*>(m_vecEffect[iSel]);

	CString cstr(pEffect->Get_EffectInfo().strName.c_str());

	m_SkillEffectListBox.AddString(cstr);
	m_MakeEffectList.push_back(pEffect);

	UpdateData(FALSE);
}


void CEffectTool::OnBnClickedButtonMakeSkilleffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	m_ComSkillEffectListBox.AddString(m_cstrSkillEffectName);

	CT2CA pszConvertedAnsiString(m_cstrSkillEffectName);
	std::string name(pszConvertedAnsiString);


	CGameObject* pObj = CSkillEffect::Create(CObjectManager::GetInstance()->GetDevice(), CObjectManager::GetInstance()->GetSrvDescriptorHeap()[HEAP_DEFAULT], CObjectManager::GetInstance()->GetCbvSrvDescriptorSize(), name);
	dynamic_cast<CSkillEffect*>(pObj)->GetEffectList() = m_MakeEffectList;
	CObjectManager::GetInstance()->Add_Object(pObj);

	m_vecEffectSkill.push_back(pObj);

	m_MakeEffectList.clear();

	UpdateData(FALSE);
}

void CEffectTool::OnLbnSelchangeListCompleteSkillEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	
	int iSel = m_ComSkillEffectListBox.GetCurSel();
	if (iSel > m_vecEffectSkill.size())
		return;

	if (m_pCurObject)
	{
		m_pCurObject->Set_Enable(false);
		m_pCurObject = nullptr;
	}

	m_pCurObject = m_vecEffectSkill[iSel];
	m_pCurObject->Set_Enable(true);

	UpdateData(FALSE);
}

void CEffectTool::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ofstream out("Effect.txt");
	size_t iSize = m_vecEffect.size();

	out << iSize << endl;

	for (int i = 0; i < iSize; ++i)
	{
		EFFECT_INFO info = dynamic_cast<CEffect*>(m_vecEffect[i])->Get_EffectInfo();
		if (info.strMeshName == "Com_Geometry")
			info.strMeshName = "Com_Mesh_Geometry";
		
		out << info.isBillboard << '\t' << info.strName << '\t' << info.strMeshName << '\t' << info.strTexName << '\t';
		out << info.S_Pos.x << '\t' << info.S_Pos.y << '\t' << info.S_Pos.z << '\t';
		out << info.S_Size.x << '\t' << info.S_Size.y << '\t' << info.S_Size.z << '\t';
		out << info.S_Rot.x << '\t' << info.S_Rot.y << '\t' << info.S_Rot.z << '\t';
		out << info.S_Color.x << '\t' << info.S_Color.y << '\t' << info.S_Color.z << '\t' << info.S_Color.w << '\t';
		out << info.E_Pos.x << '\t' << info.E_Pos.y << '\t' << info.E_Pos.z << '\t';
		out << info.E_Size.x << '\t' << info.E_Size.y << '\t' << info.E_Size.z << '\t';
		out << info.E_Rot.x << '\t' << info.E_Rot.y << '\t' << info.E_Rot.z << '\t';
		out << info.E_Color.x << '\t' << info.E_Color.y << '\t' << info.E_Color.z << '\t' << info.E_Color.w << '\t';
		out << info.StartTime << '\t' << info.LifeTime << '\t';
		bool isFrame = dynamic_cast<CEffect*>(m_vecEffect[i])->Get_IsFrame();
		out << isFrame;
		if (isFrame)
		{
			UV_FRAME_INFO frame = dynamic_cast<CEffect*>(m_vecEffect[i])->Get_FrameInfo();
			out << '\t' << frame.f2FrameSize.x << '\t' << frame.f2FrameSize.y << '\t';
			out << frame.f2maxFrame.x << '\t' << frame.f2maxFrame.y << '\t';
			out << frame.fSpeed;
		}
		if (i < iSize - 1)
			out << endl;
	}
	out.close();

	iSize = m_vecEffectSkill.size();
	ofstream skillOut("EffectSkill.txt");

	skillOut << iSize << endl;

	for (int i = 0; i < iSize; ++i)
	{
		skillOut << dynamic_cast<CSkillEffect*>(m_vecEffectSkill[i])->GetName() << '\t';
		list<CEffect*> list = dynamic_cast<CSkillEffect*>(m_vecEffectSkill[i])->GetEffectList();
		skillOut << list.size() << '\t';
		for (auto& elem : list)
		{
			skillOut << elem->Get_EffectInfo().strName << '\t';
		}
		if (i < iSize - 1)
			skillOut << endl;
	}


	auto texmap = CTexture_Manager::GetInstance()->Get_TextureMap(HEAP_TEXTURE_EFFECT);
	ofstream texOut("EffectTexture.txt");

	iSize = texmap.size();
	texOut << iSize << endl;
	
	auto iter_end = texmap.end();
	int i = 0;
	for (auto iter = texmap.begin(); iter != iter_end; ++iter, ++i)
	{
		string filePath = "Assets/Textures/Effect/";

		texOut << iter->second->Name << '\t';
		texOut << filePath + iter->second->Name + ".dds";

		if (i < iSize - 1)
			texOut << endl;
	}

	ofstream modelOut("EffectModelList.txt");
}

void CEffectTool::OnLbnSelchangeListModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	int iSel = m_ModelListBox.GetCurSel();

	CString cstrMeshName;
	m_ModelListBox.GetText(iSel, cstrMeshName);

	CT2CA pszConvertedAnsiString(cstrMeshName);
	std::string strMesh(pszConvertedAnsiString);

	if (m_IsReplace)
	{
		EFFECT_INFO tInfo(m_SPos, m_EPos, m_SSize, m_ESize, m_SRot, m_ERot, m_SColor, m_EColor, m_fLifeTime, m_fStartTime, "", strMesh, m_ButtonIsBillboard.GetCheck());
		m_pCurObject = CEffect::Create(CObjectManager::GetInstance()->GetDevice()
			, CObjectManager::GetInstance()->GetSrvDescriptorHeap()[HEAP_DEFAULT]
			, CObjectManager::GetInstance()->GetCbvSrvDescriptorSize()
			, tInfo);

		CObjectManager::GetInstance()->Add_Object(m_pCurObject);
		CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, m_pCurObject);
		m_IsReplace = false;
	}

	else
	{
		wstring wstr = wstring(strMesh.begin(), strMesh.end());
		dynamic_cast<CEffect*>(m_pCurObject)->SetMesh(const_cast<wchar_t*>(wstr.c_str()));
	}

	UpdateData(FALSE);
}


void CEffectTool::OnBnClickedButtonShow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	int iSel = m_EffectListBox.GetCurSel();
	if (iSel > m_vecEffect.size())
		return;

	if(m_vecEffect[iSel])
	{
		m_vecEffect[iSel]->Set_Enable(!m_vecEffect[iSel]->IsEnable());
	}
	
	UpdateData(FALSE);
}


void CEffectTool::OnBnClickedButtonPlayAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	size_t iCnt = m_vecEffect.size();

	for (size_t i = 0; i < iCnt; ++i)
	{
		if (m_vecEffect[i]->IsEnable())
		{
			dynamic_cast<CEffect*>(m_vecEffect[i])->SetPlay(true);
		}
	}

	UpdateData(FALSE);
}


void CEffectTool::OnBnClickedButtonStopAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	size_t iCnt = m_vecEffect.size();

	for (size_t i = 0; i < iCnt; ++i)
	{
		if (m_vecEffect[i]->IsEnable())
		{
			dynamic_cast<CEffect*>(m_vecEffect[i])->SetPlay(false);
		}
	}

	UpdateData(FALSE);
}
