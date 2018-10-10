#include "stdafx.h"
#include "SelectScene.h"
#include "Management.h"
#include "Renderer.h"
#include "Layer.h"
#include "Terrain.h"
#include "Transform.h"
#include "Sound.h"
#include "InputDevice.h"
#include "ChangeUI.h"
#include "Skeleton.h"
#include "Texture_Manager.h"
#include "TestScene.h"
#include "StaticUI.h"
#include "HPBar.h"
#include "Terrain.h"
#include "SelectPlayer.h"

HPBar* gBar = nullptr;
HPBar* gBar2 = nullptr;
Terrain* pTer = nullptr;
CSelectScene::CSelectScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize)
	: CScene(d3dDevice, srv, srvSize)
{

}

CSelectScene::~CSelectScene()
{
}

HRESULT CSelectScene::Initialize()
{
	XMFLOAT2 move;
	XMFLOAT2 scale;
	float size = 1.f;


	//Texture* tex;
	//CGameObject* pObject;
	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("SelectBackground", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 0.f;
	scale.x = 1.0f;
	scale.y = 1.0f;
	size = 1.0f;
	CGameObject* pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true, 0.02f);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	//dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);

	pTer = Terrain::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	pTer->m_xmf3Scale = XMFLOAT3(2.f, 2.f, 2.f);
	pTer->SetTex(tex->Num);
	//pTer->m_xmf3Rot.x = XMConvertToDegrees(-90.f);
	pTer->Rotation(-90.f, 0.f,0.f);
	
	Ready_GameObject(L"Layer_SelectSceneUI", pTer);

	//move.x = -0.6f;
	//move.y = 0.f;
	//scale.x = 0.7f;
	//scale.y = 0.9f;
	//size = 0.5f;
	//tex = CTexture_Manager::GetInstance()->Find_Texture("SelectWarrior", CTexture_Manager::TEX_DEFAULT_2D);
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true, 0.01f);
	//pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	//dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.f), 1.5f);
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_SelectSceneUI", pObject);
	//m_vecSelect.push_back(dynamic_cast<ChangeUI*>(pObject));


	//tex = CTexture_Manager::GetInstance()->Find_Texture("WarriorUITex", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = -1.0f;
	//move.y = 0.f;
	//scale.x = 0.4f;
	//scale.y = 0.5f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true);
	//pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_SelectSceneUI", pObject);


	//move.x = 0.6f;
	//move.y = 0.f;
	//scale.x = 0.7f;
	//scale.y = 0.9f;
	//tex = CTexture_Manager::GetInstance()->Find_Texture("SelectWizard", CTexture_Manager::TEX_DEFAULT_2D);
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true, 0.01f);
	//pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	//dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.f), 1.5f);
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_SelectSceneUI", pObject);
	//m_vecSelect.push_back(dynamic_cast<ChangeUI*>(pObject));

	//tex = CTexture_Manager::GetInstance()->Find_Texture("MageUITex", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = 0.85f;
	//move.y = 0.f;
	//scale.x = 0.5f;
	//scale.y = 0.5f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true);
	//pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_SelectSceneUI", pObject);


	tex = CTexture_Manager::GetInstance()->Find_Texture("PressEnter", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = -1.9f;
	scale.x = 1.2f;
	scale.y = 0.4f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true);
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PressEnter", tex->Num, true);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(0.f, 0.0f, 0.f, 0.f), 1.5f);
	//dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	Ready_GameObject(L"Layer_SelectSceneUI", pObject);

	tex = CTexture_Manager::GetInstance()->Find_Texture("SelectCharacter", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 1.5f;
	scale.x = 1.5f;
	scale.y = 0.5f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true);
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_SelectCharacter", tex->Num, true);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(1.f, 1.0f, 1.0f, 1.f), 1.5f);
	//dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	Ready_GameObject(L"Layer_SelectSceneUI", pObject);

	//pObject = CSkeleton::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Warrior", true);
	// > Warrior
	pObject = SelectPlayer::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, true);
	pObject->SetCamera(Get_MainCam());
	//Ready_GameObject(L"Layer_Select", pObject);
	pObject->SetPosition(XMFLOAT3(-4.5f, 0.f, 0.f));
	pObject->SetObjectAnimState(1);
	m_vecSelect.push_back(dynamic_cast<SelectPlayer*>(pObject));

	// > Wizard
	pObject = SelectPlayer::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, false);
	pObject->SetCamera(Get_MainCam());
	//Ready_GameObject(L"Layer_Select", pObject);
	pObject->SetPosition(XMFLOAT3(4.5f, 0.f, 0.f));
	m_vecSelect.push_back(dynamic_cast<SelectPlayer*>(pObject));

	// > Sound
	//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Combat_Music_06");

	//pObject = Player::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, true);
	//pObject->SetCamera(Get_MainCam());
	//pObject->SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
	//Ready_GameObject(L"Layer_SelectPlayer", pObject);
	


	// > UISetting Test
	// > ===================================================================================
	vector<ChangeUI*>	vec;
	tex = CTexture_Manager::GetInstance()->Find_Texture("light_004", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 0.3f;
	scale.x = 1.5f;
	scale.y = 1.5f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Seletor0", tex->Num, true);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	UV_FRAME_INFO info;
	info.f2maxFrame = XMFLOAT2(5.f, 5.f);
	info.fSpeed = 20.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	Ready_GameObject(L"Layer_SelectSceneUI", pObject);


	//tex = CTexture_Manager::GetInstance()->Find_Texture("wing_001", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = 0.f;
	//move.y = 0.6f;
	//scale.x = 1.4f;
	//scale.y = 1.f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false , 0.003f, 0.8f);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	//info.f2maxFrame = XMFLOAT2(5.f, 3.f);
	//info.fSpeed = 15.f;
	//dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	//vec.push_back(dynamic_cast<ChangeUI*>(pObject));


	//tex = CTexture_Manager::GetInstance()->Find_Texture("magic_003", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = -0.2f;
	//move.y = 0.17f;
	//scale.x = 0.5f;
	//scale.y = 0.5f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false , 0.001f);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	//info.f2maxFrame = XMFLOAT2(5.f, 2.f);
	//info.fSpeed = 10.f;
	//dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	////vec.push_back(dynamic_cast<ChangeUI*>(pObject));


	//tex = CTexture_Manager::GetInstance()->Find_Texture("LevelUpUI", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = 0.f;
	//move.y = 0.53f;
	//scale.x = 0.85f;
	//scale.y = 0.64f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false , 0.004f, 1.4f);
	//pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	//dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(0.8f, 0.4f, 0.8f, 0.3f), 1.f);
	//vec.push_back(dynamic_cast<ChangeUI*>(pObject));

	//tex = CTexture_Manager::GetInstance()->Find_Texture("effect_008", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = 0.25f;
	//move.y = 0.59f;
	//scale.x = 0.8f;
	//scale.y = 0.8f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.0f, 1.4f);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	//info.f2maxFrame = XMFLOAT2(5.f, 10.f);
	//info.fSpeed = 30.f;
	//dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	//vec.push_back(dynamic_cast<ChangeUI*>(pObject));

	//CManagement::GetInstance()->SetLevelUPUI(vec);

	////===================================atasageds-===============================

	//vector<ChangeUI*>	vec2;
	//tex = CTexture_Manager::GetInstance()->Find_Texture("Heal_002", CTexture_Manager::TEX_DEFAULT_2D);
	//move.x = 0.f;
	//move.y = -1.25f;
	//scale.x = 0.6f;
	//scale.y = 0.6f;
	//move.x = 0.35f;
	//pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.0f);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	//info.f2maxFrame = XMFLOAT2(5.f, 5.f);
	//info.fSpeed = 20.f;
	//dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	//vec2.push_back(dynamic_cast<ChangeUI*>(pObject));

	//CManagement::GetInstance()->SetSkillUPUI(vec2);

	//move.x = -0.3f;
	//move.y = 7.3f;

	//scale.x = 1.2f;
	//scale.y = 0.125f;
	//size = 0.25f;

	//tex = CTexture_Manager::GetInstance()->Find_Texture("BloodTex", CTexture_Manager::TEX_DEFAULT_2D);


	//move.x = -0.82f;
	//move.y = 0.75f;

	//scale.x = 1.0f;
	//scale.y = 1.0f;

	//size = 0.125f;

	//tex = CTexture_Manager::GetInstance()->Find_Texture("WarriorUITex", CTexture_Manager::TEX_DEFAULT_2D);

	////pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_WarriorUI", tex->Num);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_PlayerStateUI", pObject);
	////	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_UI, pObject);

	//move.x = -0.5f;
	//move.y = 1.45f;

	//scale.x = 1.0f;
	//scale.y = 0.5f;

	//size = 0.5f;

	//tex = CTexture_Manager::GetInstance()->Find_Texture("BarUI", CTexture_Manager::TEX_DEFAULT_2D);

	////pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerHPState", tex->Num);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_PlayerHPStateUI", pObject);


	////XMFLOAT2 move = XMFLOAT2(-0.3f, 7.3f);

	move.x = 0.f;
	move.y = -10.1391f;
	//move = XMFLOAT2(0.0f, 0.f);
	//XMFLOAT2 scale = XMFLOAT2(1.2f, 0.125f);
	scale.x = 5.f;
	scale.y = 0.0631968f;
	size = 0.543387f;

	//tex = CTexture_Manager::GetInstance()->Find_Texture("HPUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	//// > Hp Bar
	//gBar = HPBar::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num);
	////pObject->SetCamera(CManagement::GetInstance()->Get_CurScene()->Get_MainCam());
	//Ready_GameObject(L"Layer_PlayerStateUI", gBar);
	//gBar->GetCur() = 200.f;
	//gBar->GetMax() = 200.f;

	//tex = CTexture_Manager::GetInstance()->Find_Texture("HPUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	////pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerHPState", tex->Num);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_PlayerHPStateUI", pObject);

	//tex = CTexture_Manager::GetInstance()->Find_Texture("ExpUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	////pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerExpState", tex->Num);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_PlayerHPStateUI", pObject);
	//tex = CTexture_Manager::GetInstance()->Find_Texture("SkillNot", CTexture_Manager::TEX_DEFAULT_2D);

	////// > Hp Bar
	//move.x = 0.f;
	//move.y = 0.f;

	//scale.x = 0.05f;
	//scale.y = 2.f;
	//size = 0.08f;

	//gBar = HPBar::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num);
	////pObject->SetCamera(CManagement::GetInstance()->Get_CurScene()->Get_MainCam());
	//Ready_GameObject(L"Layer_PlayerStateUI", gBar);
	//gBar->GetCur() = 200.f;
	//gBar->GetMax() = 200.f;
	//tex = CTexture_Manager::GetInstance()->Find_Texture("GageUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	////pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerGageState", tex->Num);
	//pObject->SetCamera(Get_MainCam());
	//dynamic_cast<StaticUI*>(pObject)->SetColor(1.f, 1.f, 1.f, 0.7f);
	//Ready_GameObject(L"Layer_PlayerStateUI", pObject);
	tex = CTexture_Manager::GetInstance()->Find_Texture("SkillUIWarrior", CTexture_Manager::TEX_DEFAULT_2D);

	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_SkillUI", tex->Num);
	//pObject->SetCamera(Get_MainCam());
	////dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	//Ready_GameObject(L"Layer_PlayerHPStateUI", pObject);
	//// > ===================================================================================

	//size = 1.f;
	//scale.y = 1.f;
	//move.x = -0.00333446f;
	//move.y = -0.553653f;
	//tex = CTexture_Manager::GetInstance()->Find_Texture("Warrior_1", CTexture_Manager::TEX_DEFAULT_2D);
	//gBar = HPBar::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num);
	//gBar->GetCur() = 2.f;
	//gBar->GetMax() = 2.f;
	//gBar->SetColor(1.f, 1.f, 1.f, 1.f);
	//Ready_GameObject(L"Layer_PlayerHPStateUI", gBar);

	return S_OK;
}

bool CSelectScene::Update(const GameTimer & gt)
{
	CScene::Update(gt);

	if (m_isSelect)
	{
		m_fSelectTime += gt.DeltaTime();

		int iSize = m_vecSelect.size();
		for (int i = 0; i < iSize; ++i)
		{
			m_vecSelect[i]->Update(gt);
		}

		if (m_fSelectTime > 2.3f)
		{
			CScene* pScene = CTestScene::Create(m_d3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize, m_isLeft);

			pScene->Set_MainCam(m_pMainCam);
			pScene->Set_CamFrustum(m_pCamFrustum);
			m_pMainCam->Set_Dynamic(false);
			CManagement::GetInstance()->Change_Scene(pScene);
			CManagement::GetInstance()->GetSound()->StopBGM();
		}
		return true;
	}

	int iSize = m_vecSelect.size();
	for (int i = 0; i < iSize; ++i)
	{
		m_vecSelect[i]->Update(gt);
	}

	if (KeyBoard_Input(DIK_RIGHT) == CInputDevice::INPUT_PRESS)
	{
		// 얘는 법사
		m_isLeft = false;
		m_vecSelect[0]->SetObjectAnimState(0);
		m_vecSelect[1]->SetObjectAnimState(1);
		CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Move");
	}

	if (KeyBoard_Input(DIK_LEFT) == CInputDevice::INPUT_PRESS)
	{
		// > 여기가 워리어
		m_vecSelect[0]->SetObjectAnimState(1);
		m_vecSelect[1]->SetObjectAnimState(0);
		m_isLeft = true;
		CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Move");
	}

	if (KeyBoard_Input(DIK_RETURN) == CInputDevice::INPUT_PRESS)
	{
		if (m_isLeft)
			m_vecSelect[0]->SetObjectAnimState(2);
		else
			m_vecSelect[1]->SetObjectAnimState(2);
		/*CScene* pScene = CTestScene::Create(m_d3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize, m_isLeft);

		pScene->Set_MainCam(m_pMainCam);
		pScene->Set_CamFrustum(m_pCamFrustum);
		m_pMainCam->Set_Dynamic(false);
		CManagement::GetInstance()->Change_Scene(pScene);
		CManagement::GetInstance()->GetSound()->StopBGM();*/
		m_isSelect = true;
	}
	return true;
}

void CSelectScene::Render(ID3D12GraphicsCommandList * cmdList)
{
}

CSelectScene * CSelectScene::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>>& srv, UINT srvSize)
{
	CSelectScene* pInstance = new CSelectScene(d3dDevice, srv, srvSize);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CSelectScene Creatd Failed");
	}

	return pInstance;
}

void CSelectScene::Free()
{
	CScene::Free();
}
