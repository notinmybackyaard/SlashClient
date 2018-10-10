#include "stdafx.h"
#include "StartScene.h"
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
#include "SelectScene.h"
#include "Terrain.h"
#include "SelectPlayer.h"

CStartScene::CStartScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize)
	: CScene(d3dDevice, srv, srvSize)
{

}

CStartScene::~CStartScene()
{
}

HRESULT CStartScene::Initialize()
{
	XMFLOAT2 move;
	XMFLOAT2 scale;
	float size = 1.f;


	//Texture* tex;
	//CGameObject* pObject;
	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("StartScene", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 0.f;
	scale.x = 1.0f;
	scale.y = 1.0f;
	size = 1.0f;
	CGameObject* pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, true, 0.02f);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	Ready_GameObject(L"Layer_SelectSceneUI", pObject);


	tex = CTexture_Manager::GetInstance()->Find_Texture("Slash", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = -0.f;
	scale.x = 0.7f;
	scale.y = 0.4f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetPlay(true);
	dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 1.5f);
	dynamic_cast<ChangeUI*>(pObject)->SetInitColor(XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f));
	dynamic_cast<ChangeUI*>(pObject)->SetColor(XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f));
	dynamic_cast<ChangeUI*>(pObject)->SetRemain(true);
	//dynamic_cast<CInstancingObject*>(pObject)->SetCamFrustum(mCamFrustum);
	Ready_GameObject(L"Layer_SelectSceneUI", pObject);

	//pObject = CSkeleton::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Warrior", true);
	// > Warrior
	/*pObject = SelectPlayer::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, true);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Select", pObject);
	pObject->SetPosition(XMFLOAT3(-4.5f, 0.f, 0.f));
	m_vecSelect.push_back(dynamic_cast<SelectPlayer*>(pObject));
	*/
	// > Wizard
	/*pObject = SelectPlayer::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, false);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Select", pObject);
	pObject->SetPosition(XMFLOAT3(4.5f, 0.f, 0.f));
	m_vecSelect.push_back(dynamic_cast<SelectPlayer*>(pObject));*/

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
	scale.x = 1.3f;
	scale.y = 1.3f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.002f);
	pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	UV_FRAME_INFO info;
	info.f2maxFrame = XMFLOAT2(5.f, 5.f);
	info.fSpeed = 20.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	vec.push_back(dynamic_cast<ChangeUI*>(pObject));


	tex = CTexture_Manager::GetInstance()->Find_Texture("wing_001", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 0.6f;
	scale.x = 1.2f;
	scale.y = 0.8f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.003f, 0.8f);
	pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	info.f2maxFrame = XMFLOAT2(5.f, 3.f);
	info.fSpeed = 15.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	vec.push_back(dynamic_cast<ChangeUI*>(pObject));


	tex = CTexture_Manager::GetInstance()->Find_Texture("magic_003", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = -0.2f;
	move.y = 0.17f;
	scale.x = 0.3f;
	scale.y = 0.3f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.001f);
	pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	info.f2maxFrame = XMFLOAT2(5.f, 2.f);
	info.fSpeed = 10.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	//vec.push_back(dynamic_cast<ChangeUI*>(pObject));


	tex = CTexture_Manager::GetInstance()->Find_Texture("LevelUpUI", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = 0.53f;
	scale.x = 0.65f;
	scale.y = 0.54f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.004f, 1.4f);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);
	dynamic_cast<ChangeUI*>(pObject)->SetChangeInfo(XMFLOAT4(0.8f, 0.4f, 0.8f, 0.3f), 1.f);
	vec.push_back(dynamic_cast<ChangeUI*>(pObject));

	tex = CTexture_Manager::GetInstance()->Find_Texture("effect_008", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.25f;
	move.y = 0.59f;
	scale.x = 0.6f;
	scale.y = 0.6f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.0f, 1.4f);
	pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	info.f2maxFrame = XMFLOAT2(5.f, 10.f);
	info.fSpeed = 30.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	vec.push_back(dynamic_cast<ChangeUI*>(pObject));

	CManagement::GetInstance()->SetLevelUPUI(vec);

	//===================================atasageds-===============================

	vector<ChangeUI*>	vec2;
	tex = CTexture_Manager::GetInstance()->Find_Texture("Heal_002", CTexture_Manager::TEX_DEFAULT_2D);
	move.x = 0.f;
	move.y = -2.5f;
	scale.x = 0.3f;
	scale.y = 0.3f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.0f);
	pObject->SetCamera(Get_MainCam());
	//dynamic_cast<ChangeUI*>(pObject)->SetisChange(true);

	info.f2maxFrame = XMFLOAT2(5.f, 5.f);
	info.fSpeed = 20.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	vec2.push_back(dynamic_cast<ChangeUI*>(pObject));

	move.x = 0.73f;
	pObject = ChangeUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num, false, 0.0f);
	info.f2maxFrame = XMFLOAT2(5.f, 5.f);
	info.fSpeed = 20.f;
	dynamic_cast<ChangeUI*>(pObject)->SetFrameInfo(info);
	vec2.push_back(dynamic_cast<ChangeUI*>(pObject));

	CManagement::GetInstance()->SetSkillUPUI(vec2);

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

bool CStartScene::Update(const GameTimer & gt)
{
	CScene::Update(gt);

	m_fChangeTime += gt.DeltaTime();
	if(m_fChangeTime > 4.f)
	{
		//CScene* pScene = CTestScene::Create(m_d3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize, m_isLeft);
		CScene* pScene = CSelectScene::Create(m_d3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize);

		pScene->Set_MainCam(m_pMainCam);
		pScene->Set_CamFrustum(m_pCamFrustum);
		m_pMainCam->Set_Dynamic(false);
		CManagement::GetInstance()->Change_Scene(pScene);
		CManagement::GetInstance()->GetSound()->StopBGM();
	}

	return true;
}

void CStartScene::Render(ID3D12GraphicsCommandList * cmdList)
{
}

CStartScene * CStartScene::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>>& srv, UINT srvSize)
{
	CStartScene* pInstance = new CStartScene(d3dDevice, srv, srvSize);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CStartScene Creatd Failed");
	}

	return pInstance;
}

void CStartScene::Free()
{
	CScene::Free();
}
