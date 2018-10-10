#include "stdafx.h"
#include "TestScene.h"
#include "GameObject.h"
#include "Player.h"
#include "Define.h"
#include "Barrel.h"
#include "SkyBox.h"
#include "InstancingObject.h"
#include "Spider.h"
#include "NagaGuard.h"
#include "RockWarrior.h"
#include "TreeGuard.h"
#include "MushRoom.h"
#include "Layer.h"
#include "Management.h"
#include "Renderer.h"
#include "Terrain.h"
//#include "Collision_Manager.h"
#include "Dragon.h"
#include "Skeleton.h"
#include "Network.h"
#include "UI.h"
#include "HPBar.h"
#include "StaticUI.h"
#include "Mesh.h"
#include "MapObject.h"
#include "Texture_Manager.h"
#include "Transform.h"
#include "SkillEffect.h"
#include "Effect_Manager.h"
#include "NumUI_Inst.h"
#include "Define.h"
#include "InputDevice.h"
#include "d3dApp.h"
#include "NumUI.h"

CTestScene::CTestScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize, bool isWarrior)
	: CScene(d3dDevice, srv, srvSize)
	, m_IsWarrior(isWarrior)
{
}

CTestScene::~CTestScene()
{
}

HRESULT CTestScene::Initialize()
{
	BYTE playerType{};
	if (m_IsWarrior)
		playerType = PlayerType::PLAYER_WARRIOR;
	else
		playerType = PlayerType::PLAYER_WIZARD;

	CNetwork::GetInstance()->InitSock(D3DApp::GetApp()->MainWnd());
	CNetwork::GetInstance()->SendPlayerInitData(playerType);

	CManagement::GetInstance()->GetSound()->PlayBGM(L"Sound", L"village");

	CGameObject* pObject = SkyBox::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	Ready_GameObject(L"Layer_SkyBox", pObject);
	//CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_PRIORITY, pObject);

	pObject = Player::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, m_IsWarrior);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Player", pObject);

	for(int i = 0; i < NUM_OF_PLAYER; ++i)
	{
		pObject = CSkeleton::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Warrior", true);
		pObject->SetCamera(Get_MainCam());
		Ready_GameObject(L"Layer_Skeleton", pObject);
		//CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);
	}
	for (int i = 0; i < NUM_OF_PLAYER; ++i)
	{
		pObject = CSkeleton::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Mage", false);
		pObject->SetCamera(Get_MainCam());
		Ready_GameObject(L"Layer_Skeleton", pObject);
		//CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);
	}
	
	for (int i = 0; i < NUM_OF_NPC_TOTAL; ++i)
	{
		if (i < NAGAGUARD_ID_START)
			pObject = Spider::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
		else if (i < ROCKWARRIOR_ID_START)
			pObject = NagaGuard::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
		else if (i < TREEGUARD_ID_START)
			pObject = RockWarrior::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
		else if (i < TURTLE_ID_START)
			pObject = TreeGuard::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
		else
			pObject = Mushroom::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
		pObject->SetCamera(Get_MainCam());
		pObject->SetPosition(0, 0, 0);
		Ready_GameObject(L"Layer_Monster", pObject);
	}

	//pObject = Dragon::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	//pObject->SetCamera(Get_MainCam());
	//pObject->SetPosition(0, 0, 15);
	//Ready_GameObject(L"Layer_Dragon", pObject);

	//pObject = NagaGuard::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	//pObject->SetCamera(Get_MainCam());
	//pObject->SetPosition(0, 0, 0);
	//Ready_GameObject(L"Layer_NagaGuard", pObject);

	//pObject = RockWarrior::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	//pObject->SetCamera(Get_MainCam());
	//pObject->SetPosition(20, 0, 0);
	//Ready_GameObject(L"Layer_RockWarrior", pObject);

	//pObject = TreeGuard::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	//pObject->SetCamera(Get_MainCam());
	//pObject->SetPosition(40, 0, 0);
	//Ready_GameObject(L"Layer_TreeGuard", pObject);

	//pObject = Mushroom::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	//pObject->SetCamera(Get_MainCam());
	//pObject->SetPosition(60, 0, 0);
	//Ready_GameObject(L"Layer_Mushroom", pObject);

	pObject = Terrain::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Terrain", pObject);
	//CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, pObject);
	for (int i = 0; i < 30; ++i)
	{
		
			for (int k = 0; k < 30; ++k)
			{
				pObject = Terrain::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize);
				pObject->SetCamera(Get_MainCam());
				pObject->SetPosition(i * 50 -750, 0, k * 50 - 750);

				Ready_GameObject(L"Layer_Terrain", pObject);
			}
		

	}

	if (FAILED(Load_Map()))
		return E_FAIL;

	UISetting();

	


	return S_OK;
}

bool CTestScene::Update(const GameTimer & gt)
{

	auto * m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player");

	auto m_pCamera = CManagement::GetInstance()->Get_MainCam();

	if (m_pCamera != NULL)
	{
		m_pCamera->CameraEffect_Shaking();
		m_pCamera->CameraEffect_Damage();
		if (m_pCamera->Target != NULL)
		{
			m_pCamera->CameraEffect_ZoomIn();
			m_pCamera->CameraEffect_ZoomIn_Round();
			m_pCamera->CameraEffect_ZoomIn_RoundUltimate();
		}

	}

	auto monsterList = CManagement::GetInstance()->Get_Layer(L"Layer_Monster")->Get_ObjectList();
	m_pPlayer->m_pCollider = NULL;


	for (int i = 0; i < monsterList.size(); ++i)
	{
		monsterList[i]->m_pCollider = NULL;
	}
	CScene::Update(gt);

	CollisionProcess();
	UpdateOOBB();
	// > Player 관련 UI는 Player.cpp에서 실행
	//UpdateUI();

	m_pPlayer->m_pCollider;

	//CollisionProcess();
	if (m_pPlayer->m_pCollider == NULL)
	{
		m_pPlayer->m_MovingRefletVector = XMFLOAT3(0, 0, 0);
		//cout << "충돌안함" << endl;
	}
	else
	{
		m_pPlayer->m_pCollider->SaveSlidingVector(m_pPlayer,m_pPlayer->m_pCollider);
		
	}


	for (int i = 0; i < monsterList.size(); ++i)
	{
		if (monsterList[i]->m_pCollider == NULL)
		{
			monsterList[i]->m_MovingRefletVector = XMFLOAT3(0, 0, 0);
		}
		else
		{
			monsterList[i]->m_pCollider->SaveSlidingVector(monsterList[i], monsterList[i]->m_pCollider);

		}
	}
	return true;
}

void CTestScene::UpdateOOBB()
{
	auto * m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player");
	auto * m_pSpider = CManagement::GetInstance()->Find_Object(L"Layer_Monster");

	/*cout << " ------------------------------" << endl;
	cout << "플레이어" << m_pPlayer->m_xmOOBB.Orientation.x << "\t" << m_pPlayer->m_xmOOBB.Orientation.y << "\t" << m_pPlayer->m_xmOOBB.Orientation.z << "\t" << m_pPlayer->m_xmOOBB.Orientation.w << endl;
	cout << "거미" << m_pSpider->m_xmOOBB.Orientation.x << "\t" << m_pSpider->m_xmOOBB.Orientation.y << "\t" << m_pSpider->m_xmOOBB.Orientation.z << "\t" << m_pSpider->m_xmOOBB.Orientation.w << endl;
	cout << " ------------------------------" << endl;*/
	//cout << "거미" << m_pSpider->m_xmOOBB.Orientation.x << "\t" << m_pSpider->m_xmOOBB.Orientation.y << "\t" << m_pSpider->m_xmOOBB.Orientation.z << "\t" << m_pSpider->m_xmOOBB.Orientation.w << endl;

	//float ceta = acos(m_pSpider->m_xmOOBB.Orientation.w) * 2.0f;


	//cout << "거미" << "\t" << asin(m_pSpider->m_xmOOBB.Orientation.x) / sin(ceta/2) << "\t" << m_pSpider->m_xmOOBB.Orientation.y / sin(ceta / 2) << "\t" << m_pSpider->m_xmOOBB.Orientation.z / sin(ceta / 2) << "\t" << ceta * 57.3248f << endl;


	//Find_Object 한번만 하도록 바꿀것
	/*auto * m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player");
	auto * m_pBarrel = CManagement::GetInstance()->Find_Object(L"Layer_Barrel");
	auto * m_pInstance = CManagement::GetInstance()->Find_Object(L"Layer_Map");
	auto * m_pSpider = CManagement::GetInstance()->Find_Object(L"Layer_Monster");
	auto * m_pSpider2 = CManagement::GetInstance()->Find_Object(L"Layer_Monster",1);*/
	//auto * m_pDragon = CManagement::GetInstance()->Find_Object(L"Layer_Dragon");

	//플레이어 없는 관계로 주석
	//m_pPlayer->m_xmOOBBTransformed.Transform(m_pPlayer->m_xmOOBB, XMLoadFloat4x4(&(m_pPlayer->GetWorld())));
	//XMStoreFloat4(&m_pPlayer->m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_pPlayer->m_xmOOBBTransformed.Orientation)));


	//cout << m_pSpider->m_xmOOBB.Center.x << "\t" << m_pSpider->m_xmOOBB.Center.y << "\t" <<  m_pSpider->m_xmOOBB.Center.z << endl;

	/*auto mat1 = m_pSpider->GetWorld();
	m_pSpider->m_xmOOBBTransformed.Transform(m_pSpider->m_xmOOBB, XMLoadFloat4x4(&(m_pSpider->GetWorld())));
	XMStoreFloat4(&m_pSpider->m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_pSpider->m_xmOOBBTransformed.Orientation)));

	auto mat2 = m_pSpider2->GetWorld();

	m_pSpider2->m_xmOOBBTransformed.Transform(m_pSpider2->m_xmOOBB, XMLoadFloat4x4(&(m_pSpider2->GetWorld())));
	XMStoreFloat4(&m_pSpider2->m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_pSpider2->m_xmOOBBTransformed.Orientation)));*/


	/*m_pDragon->m_xmOOBBTransformed.Transform(m_pDragon->m_xmOOBB, XMLoadFloat4x4(&(m_pDragon->GetWorld())));
	XMStoreFloat4(&m_pDragon->m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_pDragon->m_xmOOBBTransformed.Orientation)));*/


}
void CTestScene::UpdateUI()
{
	auto * m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player");
	auto * m_pHPBar = CManagement::GetInstance()->Find_Object(L"Layer_HPBar");

	m_pHPBar->SetHp(m_pPlayer->GetHp());


	//cout << m_pPlayer->m_MovingRefletVector.x << "\t" << m_pPlayer->m_MovingRefletVector.y << "\t" << m_pPlayer->m_MovingRefletVector.z << endl;

}
void CTestScene::Render(ID3D12GraphicsCommandList * cmdList)
{
}

void CTestScene::CollisionProcess()
{
	

	//auto * m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player");
	//auto * m_pBarrel = CManagement::GetInstance()->Find_Object(L"Layer_Barrel");

	//auto * m_pInstance = CManagement::GetInstance()->Find_Object(L"Layer_Map");
	////auto * m_pSpider = CManagement::GetInstance()->Find_Object(L"Layer_Monster");

	//vector<CGameObject *> m_pSpider;
	//for (int i = 0; i < 2; ++i)
	//{
	//	m_pSpider.push_back(CManagement::GetInstance()->Find_Object(L"Layer_Monster", i));
	//}

	////auto * m_pDragon = CManagement::GetInstance()->Find_Object(L"Layer_Dragon");


	//auto instanceData = dynamic_cast<CInstancingObject*>(m_pInstance)->GetvecInstances();


	//auto m_pCamera = CManagement::GetInstance()->Get_MainCam();


	//for (int i = 0; i < instanceData.size(); ++i)
	//{
	//	XMMATRIX world = XMLoadFloat4x4(&(instanceData[i].World));

	//	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	//	// View space to the object's local space.

	//	BoundingOrientedBox mLocalPlayerBounds;	
	//	// Transform the camera frustum from view space to the object's local space.
	//	m_pPlayer->m_xmOOBB.Transform(mLocalPlayerBounds, invWorld);

	//	// Perform the box/frustum intersection test in local space.
	//	if (mLocalPlayerBounds.Intersects(m_pInstance->GetBounds()) != DirectX::DISJOINT)
	//	{

	//		//cout << i << "번째 인스턴싱 오브젝트랑 충돌" << endl;
	//	}

	//}


	////if (m_pPlayer->m_xmOOBB.Intersects(m_pBarrel->m_xmOOBB))
	////{
	////	//cout << " Barrel 충돌 " << endl;
	////}
	////else
	////{
	////	//cout << " Barrel 충돌 아님" << endl;
	////}

	////cout << m_pSpider->m_xmOOBB.Extents.x << "\t" << m_pSpider->m_xmOOBB.Extents.y <<"\t"<< m_pSpider->m_xmOOBB.Extents.z << endl;
	////for (int i = 0; i < 2; ++i)
	////{

	////	if (m_pPlayer->m_xmOOBB.Intersects(m_pSpider[i]->m_xmOOBB))
	////	{
	////		//cout << i << "거미 충돌 " << endl;
	////		
	////		m_pSpider[i]->SetObjectAnimState(2);
	////	}
	////	else
	////	{
	////		//cout << i << "거미 충돌 아님" << endl;

	////		m_pSpider[i]->SetObjectAnimState(0);

	////	}
	////}
	//for (int i = 0; i < 2; ++i)
	//{

	//	if (m_pPlayer->m_xmOOBB.Intersects(m_pSpider[i]->m_xmOOBB))
	//	{
	//		//cout << i << "거미 충돌 " << endl;
	//		
	//		m_pSpider[i]->SetObjectAnimState(1);
	//	}
	//	else
	//	{
	//		//cout << i << "거미 충돌 아님" << endl;

	//		m_pSpider[i]->SetObjectAnimState(0);

	//	}
	//}


	////if (m_pPlayer->m_xmOOBB.Intersects(m_pDragon->m_xmOOBB))
	////{
	////	//cout << "드래곤 충돌 " << endl;
	////}
	////else
	////{
	////	//cout << "드래곤 충돌 아님" << endl;
	////}


}

HRESULT CTestScene::Load_Map()
{
	ifstream fin("Assets/Data/MapData.txt");
	if (!fin)
	{
		MSG_BOX(L"MapData 못읽음 망해따리 망해따~~~~");
		return E_FAIL;
	}
	string ignore;
	/*
	while (!fin.eof())
	{
	fin >> ignore;      // > MeshType

	Mesh::MESHTYPE eType = Mesh::MESHTYPE(std::stoi(ignore));

	fin >> ignore;      // > Com_Mesh_이름
	wstring wstrFileName;
	wstring meshName = wstrFileName.assign(ignore.begin(), ignore.end());
	CGameObject* pObject;
	int iSize = 1;
	fin >> iSize;
	CRenderer::RenderType eRenderType;
	switch (eType)
	{
	case Mesh::MESH_STATIC:
	pObject = CMapObject::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, const_cast<wchar_t*>(meshName.c_str()));
	eRenderType = CRenderer::RenderType::RENDER_NONALPHA_FORWARD;
	break;
	case Mesh::MESH_STATIC_INST:
	pObject = CInstancingObject::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_INSTANCING], mCbvSrvDescriptorSize, const_cast<wchar_t*>(meshName.c_str()), iSize);
	eRenderType = CRenderer::RenderType::RENDER_NONALPHA_INSTANCING;
	break;
	}


	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_Map", pObject);
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(eRenderType, pObject);

	std::string::size_type sz;

	string Value[9];
	float fValue[9];
	for (int i = 0; i < iSize; ++i)
	{
	for (int j = 0; j < 9; ++j)
	{
	fin >> Value[j];
	fValue[j] = std::stof(Value[j], &sz);
	}
	if (eType == (Mesh::MESH_STATIC_INST))
	{
	pObject->GetTransform(i)->Translation(fValue[0], fValue[1], fValue[2]);
	pObject->GetTransform(i)->Scaling(fValue[3], fValue[4], fValue[5]);
	pObject->GetTransform(i)->Rotation(fValue[6], fValue[7], fValue[8]);
	}
	else
	{
	pObject->GetTransform()->Translation(fValue[0], fValue[1], fValue[2]);
	pObject->GetTransform()->Scaling(fValue[3], fValue[4], fValue[5]);
	pObject->GetTransform()->Rotation(fValue[6], fValue[7], fValue[8]);
	}
	}
	}
	*/

	while (!fin.eof())
	{
		if (m_ObjectCount == 0)
			fin >> m_ObjectCount;

		fin >> ignore;      // > Com_Mesh_이름

		wstring wstrFileName;
		wstring meshName = wstrFileName.assign(ignore.begin(), ignore.end());

		CGameObject* pObject;
		CRenderer::RenderType eRenderType;

		int iSize = 1;
		fin >> iSize;         // > Instancing Size

							  //pObject = CMapObject::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, const_cast<wchar_t*>(meshName.c_str()));
							  //eRenderType = CRenderer::RenderType::RENDER_NONALPHA_FORWARD;
		pObject = CInstancingObject::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_INSTANCING], mCbvSrvDescriptorSize, const_cast<wchar_t*>(meshName.c_str()), iSize);
		eRenderType = CRenderer::RenderType::RENDER_NONALPHA_INSTANCING;
		//eRenderType = CRenderer::RenderType::RENDER_ALPHA_INST;




		pObject->SetCamera(Get_MainCam());

		std::string::size_type sz;

		string Value[9];
		float fValue[9];
		for (int i = 0; i < iSize; ++i)
		{
			int Alpha;
			fin >> Alpha;
			if (Alpha)
				eRenderType = CRenderer::RenderType::RENDER_ALPHA_INST;
			else
				eRenderType = CRenderer::RenderType::RENDER_NONALPHA_INSTANCING;
			fin >> ignore;         // > Tex_Name
			Texture* tex = CTexture_Manager::GetInstance()->Find_Texture(ignore, CTexture_Manager::TEX_INST_2D);      // > 인스턴싱일 때 바꿔줌
																													  //dynamic_cast<CInstancingObject*>(pObject)->SetTexture(i, tex->Num);

			for (int j = 0; j < 9; ++j)
			{
				fin >> Value[j];
				fValue[j] = std::stof(Value[j], &sz);
			}

			dynamic_cast<CInstancingObject*>(pObject)->GetTransform(i)->Translation(fValue[0], fValue[1], fValue[2]);
			dynamic_cast<CInstancingObject*>(pObject)->GetTransform(i)->Scaling(fValue[3], fValue[4], fValue[5]);
			dynamic_cast<CInstancingObject*>(pObject)->GetTransform(i)->Rotation(fValue[6], fValue[7], fValue[8]);
			dynamic_cast<CInstancingObject*>(pObject)->SetRenderType(eRenderType);
		}
		Ready_GameObject(L"Layer_Map", pObject);
		//CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(eRenderType, pObject);
	}


	return S_OK;
}

CTestScene * CTestScene::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize, bool isWarrior)
{
	CTestScene* pInstance = new CTestScene(d3dDevice, srv, srvSize, isWarrior);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CTestScene Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestScene::UISetting()
{
	XMFLOAT2 move = XMFLOAT2(-0.3f, 7.3f);

	move.x = -0.3f;
	move.y = 7.3f;

	XMFLOAT2 scale = XMFLOAT2(1.2f, 0.125f);
	scale.x = 1.2f;
	scale.y = 0.125f;
	float size = 0.25f;

	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("BloodTex", CTexture_Manager::TEX_DEFAULT_2D);

	CGameObject* pObject;/* = HPBar::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, size, tex->Num);
	pObject->SetCamera(Get_MainCam());
	Ready_GameObject(L"Layer_HPBar", pObject);*/
	//CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_UI, pObject);

	move.x = -0.82f;
	move.y = 0.75f;

	scale.x = 1.0f;
	scale.y = 1.0f;

	size = 0.125f;

	move.x = -0.5f;
	move.y = 1.45f;

	scale.x = 1.0f;
	scale.y = 0.5f;

	size = 0.5f;

	tex = CTexture_Manager::GetInstance()->Find_Texture("HPUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerHPState", tex->Num);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<StaticUI*>(pObject)->SetColor(1.f, 1.f, 1.f, 0.7f);
	Ready_GameObject(L"Layer_PlayerStateUI", pObject);

	tex = CTexture_Manager::GetInstance()->Find_Texture("ExpUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerExpState", tex->Num);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<StaticUI*>(pObject)->SetColor(1.f, 1.f, 1.f, 0.7f);
	Ready_GameObject(L"Layer_PlayerStateUI", pObject);

	tex = CTexture_Manager::GetInstance()->Find_Texture("GageUIBase", CTexture_Manager::TEX_DEFAULT_2D);

	//pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, move, scale, s ize, tex->Num);//, "Models/StaticMesh/staticMesh.ASE", 10);
	pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerGageState", tex->Num);
	pObject->SetCamera(Get_MainCam());
	dynamic_cast<StaticUI*>(pObject)->SetColor(1.f, 1.f, 1.f, 0.7f);
	Ready_GameObject(L"Layer_PlayerStateUI", pObject);

	tex = CTexture_Manager::GetInstance()->Find_Texture("PlayerLevelUIBack", CTexture_Manager::TEX_DEFAULT_2D);

	pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerLevelState", tex->Num);
	dynamic_cast<StaticUI*>(pObject)->SetColor(1.f, 1.f, 1.f, 0.7f);
	Ready_GameObject(L"Layer_PlayerStateUI", pObject);

	if (m_IsWarrior)
	{
		tex = CTexture_Manager::GetInstance()->Find_Texture("PlayerLevelUIWarrior", CTexture_Manager::TEX_DEFAULT_2D);
		pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerJobState", tex->Num);
		Ready_GameObject(L"Layer_PlayerStateUI", pObject);

		tex = CTexture_Manager::GetInstance()->Find_Texture("SkillUIWarrior", CTexture_Manager::TEX_DEFAULT_2D);
		pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_SkillUI", tex->Num);
		//Ready_GameObject(L"Layer_PlayerStateUI", pObject);
	}
		
	else
	{
		tex = CTexture_Manager::GetInstance()->Find_Texture("PlayerLevelUIWizard", CTexture_Manager::TEX_DEFAULT_2D);
		pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_PlayerJobState", tex->Num);
		Ready_GameObject(L"Layer_PlayerStateUI", pObject);

		tex = CTexture_Manager::GetInstance()->Find_Texture("SkillUIWizard", CTexture_Manager::TEX_DEFAULT_2D);
		pObject = StaticUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_SkillUI", tex->Num);
		//Ready_GameObject(L"Layer_PlayerStateUI", pObject);
	}
		



	tex = CTexture_Manager::GetInstance()->Find_Texture("Num_LV", CTexture_Manager::TEX_DEFAULT_2D);

	pObject = NumUI::Create(m_d3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, L"Com_Mesh_Num", tex->Num);
	Ready_GameObject(L"Layer_PlayerStateUI", pObject);
}

void CTestScene::Free()
{
	CScene::Free();
}
