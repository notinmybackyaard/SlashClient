#include "stdafx.h"
#include "NagaGuard.h"
#include "Define.h"
#include "DynamicMeshSingle.h"
#include "Camera.h"
#include "Management.h"
#include "Renderer.h"
#include "Effect_Manager.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Player.h"
#include "Layer.h"
#include "Network.h"


NagaGuard::NagaGuard(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize)
	: CGameObject(d3dDevice, srv, srvSize)
{
	m_fMoveSpeed = 2.0f;

	m_fRotateSpeed = 4.0f;
}


NagaGuard::~NagaGuard()
{
}

bool NagaGuard::Update(const GameTimer & gt)
{



	if (!m_pPlayer)
		m_pPlayer = dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player"));

	CGameObject::Update(gt);




	m_pCamera = CManagement::GetInstance()->Get_MainCam();
	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);



	//auto currObjectCB2 = m_pFrameResource->InstanceBuffer.get();

	XMMATRIX world = XMLoadFloat4x4(&World);
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);

	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	// View space to the object's local space.
	XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

	// Transform the camera frustum from view space to the object's local space.
	BoundingFrustum localSpaceFrustum;
	mCamFrustum = *CManagement::GetInstance()->Get_CurScene()->Get_CamFrustum();
	mCamFrustum.Transform(localSpaceFrustum, viewToLocal);


	// Perform the box/frustum intersection test in local space.
	if ((localSpaceFrustum.Contains(Bounds) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
	{
		//cout << "보인당!" << endl;
		m_bIsVisiable = true;
		Animate(gt);

		auto currObjectCB = m_pFrameResource->ObjectCB.get();

		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
		objConstants.MaterialIndex = Mat->MatCBIndex;

		currObjectCB->CopyData(ObjCBIndex, objConstants);

		auto currMaterialCB = m_pFrameResource->MaterialCB.get();

		XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

		MaterialConstants matConstants;
		matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;
		matConstants.FresnelR0 = Mat->FresnelR0;
		matConstants.Roughness = Mat->Roughness;
		XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

		matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

		currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);
	}
	else
	{
		//cout << "안보인당!" << endl;
		m_bIsVisiable = false;
	}

	//cout << "NagaGuard : " << endl;
	//cout << "Bounds Center: " << Bounds.Center.x << "\t" << Bounds.Center.y << "\t" << Bounds.Center.z << endl;
	//cout << "Bounds Extents: " << Bounds.Extents.x << "\t" << Bounds.Extents.y << "\t" << Bounds.Extents.z << endl;

	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);
	return true;
}

void NagaGuard::Render(ID3D12GraphicsCommandList * cmdList)
{
	if (m_bIsVisiable && m_bIsConnected)
	{
		AnimStateMachine->SetTimerTrueFalse();

		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

		auto objectCB = m_pFrameResource->ObjectCB->Resource();
		auto matCB = m_pFrameResource->MaterialCB->Resource();

		cmdList->IASetVertexBuffers(0, 1, &Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

		Mat->DiffuseSrvHeapIndex;
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + Mat->MatCBIndex*matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(4, objCBAddress);
		//cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);
		cmdList->SetGraphicsRootShaderResourceView(5, matCBAddress);

		cmdList->SetGraphicsRootDescriptorTable(7, tex);

		//	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->Indexoffset[0];

		auto pMesh = dynamic_cast<DynamicMeshSingle*>(m_pMesh);


		//int iTest = (int)pMesh->m_fTest;

		int iTest = AnimStateMachine->GetCurAnimFrame();
		int AnimaState = AnimStateMachine->GetAnimState();

		cmdList->DrawIndexedInstanced(pMesh->Indexoffset[1], 1,
			pMesh->Indexoffset[iTest] + pMesh->IndexAnimoffset[AnimaState] /*+ pMesh->IndexAnimoffset[0]*/,
			pMesh->Vertexoffset[iTest] + pMesh->VertexAnimoffset[AnimaState]/*+ pMesh->VertexAnimoffset[0]*/, 0);


		//if (m_bLODState == true)
		//{
		//	cmdList->DrawIndexedInstanced(1200, 1,
		//		0 /*+ pMesh->IndexAnimoffset[0]*/,
		//		0/*+ pMesh->VertexAnimoffset[0]*/, 0);
		//}
		//else
		//{
		//	cmdList->DrawIndexedInstanced(pMesh->Indexoffset[1], 1,
		//		pMesh->Indexoffset[iTest] + pMesh->IndexAnimoffset[AnimaState] /*+ pMesh->IndexAnimoffset[0]*/,
		//		pMesh->Vertexoffset[iTest] + pMesh->VertexAnimoffset[AnimaState]/*+ pMesh->VertexAnimoffset[0]*/, 0);

		//}



	}


}

HRESULT NagaGuard::Initialize()
{
	m_pMesh = dynamic_cast<DynamicMeshSingle*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_NagaGuard"));
	if (nullptr == m_pMesh)
		return E_FAIL;



	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("NagaGuardTex", CTexture_Manager::TEX_DEFAULT_2D);
	if (nullptr == tex)
		return E_FAIL;

	m_strTexName[SPIDER_BRICK] = "bricksTex";
	m_strTexName[SPIDER_STONE] = "stoneTex";
	m_strTexName[SPIDER_TILE] = "tileTex";
	m_strTexName[SPIDER_ICE] = "iceTex";


	wchar_t* machineName;
	machineName = L"NagaGuard";
	int test[MonsterState::MSTATE_END] = { 0, };
	AnimStateMachine = AnimateStateMachine_NagaGuard::Create(this, machineName, test, test);
	if (AnimStateMachine == nullptr)
		return E_FAIL;


	AnimStateMachine->vecAnimFrame = &(dynamic_cast<DynamicMeshSingle*>(m_pMesh)->vecAnimFrame);

	AnimStateMachine->SetAnimState(State::STATE_IDLE);

	Mat = new Material;
	Mat->Name = "SpiderMat";
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = tex->Num;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	XMStoreFloat4x4(&World, XMMatrixScaling(m_fScale, m_fScale, m_fScale)*XMMatrixRotationX(1.7f)*XMMatrixRotationZ(3.14f)*XMMatrixTranslation(0.0f, 0.0f, 0.0f));


	//XMStoreFloat4x4(&World, XMMatrixScaling(m_fScale, m_fScale, m_fScale)*XMMatrixTranslation(0.0f, 0.0f, 0.0f)*XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<DynamicMeshSingle*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["SingleMesh"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["SingleMesh"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["SingleMesh"].BaseVertexLocation;
	Bounds = Geo->DrawArgs["SingleMesh"].Bounds;



	m_xmf3Scale = XMFLOAT3(0.08f, 0.08f, 0.08f);
	m_xmf3Rot = XMFLOAT3(1.7f, 0.f, 3.14f);

	return S_OK;
}

void NagaGuard::Animate(const GameTimer & gt)
{
	AnimStateMachine->AnimationStateUpdate(gt);

}

void NagaGuard::SaveSlidingVector(CGameObject * pobj, CGameObject * pCollobj)
{


	//XMFLOAT3 pCollObjPos2= pCollobj->m_pTransCom->m_f3Position;
	XMFLOAT3 Center = pCollobj->GetPosition();
	XMFLOAT3 Player = pobj->GetPosition();
	XMFLOAT3 dirVector = Vector3::Subtract(Player, Center);   // 충돌 객체에서 플레이어로 가는 벡터

	float tanceta = dirVector.z / dirVector.x;
	float ceta = atan(tanceta) * 57.3248f;

	float extenttanceta = pCollobj->m_xmOOBB.Extents.z / pCollobj->m_xmOOBB.Extents.x;


	float extentceta = atan(extenttanceta) * 57.3248f;


	dirVector = Vector3::Normalize(dirVector);
	//cout << "Coll obj Look" << "\t" << look.x << "\t" << look.y << "\t" << look.z << endl;
	//cout << "dirVector : " << "\t" << dirVector.x << "\t" << dirVector.y << "\t" << dirVector.z << endl;
	//cout << "Ceta : " << ceta << endl;
	//cout << "ExtentCeta : " << extentceta << endl;

	if (Player.x > Center.x && 0 < ceta && ceta< extentceta) // 1
	{
		//cout << " 오른쪽 충돌" << endl;
		pobj->m_MovingRefletVector = XMFLOAT3(1, 0, 0);


	}
	if (Player.z > Center.z && extentceta <ceta && ceta < 90) // 2
	{
		//cout << "윗쪽 충돌" << endl;
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, 1);
	}
	if (Player.z > Center.z && -90 < ceta && ceta < -extentceta) // 3
	{
		//cout << "윗쪽 충돌" << endl;
		//pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, 1);

	}
	if (Player.x < Center.x && -extentceta < ceta && ceta < 0)// 4
	{
		//cout << "왼쪽 충돌" << endl;
		//pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
		pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);

	}
	if (Player.x < Center.x && 0 < ceta && ceta < extentceta) // 5
	{
		//cout << "왼쪽 충돌" << endl;
		//pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
		pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);

	}
	if (Player.z < Center.z && extentceta < ceta && ceta < 90) // 6
	{
		//cout << "아래쪽 충돌" << endl;
		//pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, -1);

	}
	if (Player.z < Center.z && -90 < ceta && ceta < -extentceta) // 7
	{
		//cout << "아래쪽 충돌" << endl;
		//pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, -1);

	}
	if (Player.x > Center.x && -extentceta < ceta && ceta < 0) // 8
	{
		//cout << "오른쪽 충돌" << endl;
		//pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
		pobj->m_MovingRefletVector = XMFLOAT3(1, 0, 0);

	}
}

void NagaGuard::Hit(float fDamage)
{
	CEffect_Manager::GetInstance()->Play_SkillEffect("Hit");
}

//void NagaGuard::SetTexture(SpiderTex _tex)
//{
//	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture(m_strTexName[_tex], CTexture_Manager::TEX_DEFAULT_2D);
//	if (nullptr == tex)
//		return;
//	Mat->DiffuseSrvHeapIndex = tex->Num;
//
//}

NagaGuard * NagaGuard::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
{
	NagaGuard* pInstance = new NagaGuard(d3dDevice, srv, srvSize);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Player Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

// > ---------------------------- StateMachine_NagaGuard -----------------------------------

AnimateStateMachine_NagaGuard::AnimateStateMachine_NagaGuard(CGameObject * pObj, wchar_t * pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END])
	: m_pMachineName(pMachineName)
	, m_pObject(pObj)
{
}

AnimateStateMachine_NagaGuard::~AnimateStateMachine_NagaGuard()
{
}

HRESULT AnimateStateMachine_NagaGuard::Initialize()
{
	return S_OK;
}

void AnimateStateMachine_NagaGuard::AnimationStateUpdate(const GameTimer & gt)
{
	if (bTimerIdle == true)
	{
		m_fAnimationKeyFrameIndex += gt.DeltaTime() * 25;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex;
		if (m_fAnimationKeyFrameIndex > (*vecAnimFrame)[MonsterState::MSTATE_IDLE])
		{
			bTimerIdle = false;

			m_fAnimationKeyFrameIndex = 0;
		}
	}


	if (bTimerWalk == true)
	{
		m_fAnimationKeyFrameIndex_Walk += gt.DeltaTime() * 45;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Walk;
		if (m_fAnimationKeyFrameIndex_Walk > (*vecAnimFrame)[MonsterState::MSTATE_WALK])
		{
			bTimerWalk = false;
			m_fAnimationKeyFrameIndex_Walk = 0;
		}

	}


	if (bTimerAttack1 == true)
	{

		m_fAnimationKeyFrameIndex_Attack1 += gt.DeltaTime() * 20;

		if (!m_IsSoundPlay[MonsterState::MSTATE_ATTACK1] && m_fAnimationKeyFrameIndex_Attack1 > m_SoundFrame[MonsterState::MSTATE_ATTACK1])
		{
			m_IsSoundPlay[MonsterState::MSTATE_ATTACK1] = true;
			CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"NagaGuard_Attack1_Sound");
		}

		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack1;
	

		if (!m_IsEffectPlay[MonsterState::MSTATE_ATTACK1] && m_fAnimationKeyFrameIndex_Attack1 > m_EffectFrame[MonsterState::MSTATE_ATTACK1])
		{
			m_IsEffectPlay[MonsterState::MSTATE_ATTACK1] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			//cout << "스킬!" << endl;
			CEffect_Manager::GetInstance()->Play_SkillEffect("Warrior_Turn", &m_pObject->GetWorld());
			//cout << "Player Pos : " << m_pObject->GetPosition().x << ", " << m_pObject->GetPosition().y << ", " << m_pObject->GetPosition().z << endl;
		}

		if (m_fAnimationKeyFrameIndex_Attack1 > (*vecAnimFrame)[MonsterState::MSTATE_ATTACK1])
		{
			bTimerAttack1 = false;
			m_fAnimationKeyFrameIndex_Attack1 = 0.f;

			m_IsSoundPlay[MonsterState::MSTATE_ATTACK1] = false;
			m_IsEffectPlay[MonsterState::MSTATE_ATTACK1] = false;
		}

	}


	if (bTimerAttack2 == true)
	{

		m_fAnimationKeyFrameIndex_Attack2 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;

		if (!m_IsSoundPlay[MonsterState::MSTATE_ATTACK2] && m_fAnimationKeyFrameIndex_Attack2 > m_SoundFrame[MonsterState::MSTATE_ATTACK2])
		{
			m_IsSoundPlay[MonsterState::MSTATE_ATTACK2] = true;
			CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK2]);
		}

		if (!m_IsEffectPlay[MonsterState::MSTATE_ATTACK2] && m_fAnimationKeyFrameIndex_Attack2 > m_EffectFrame[MonsterState::MSTATE_ATTACK2])
		{
			m_IsEffectPlay[MonsterState::MSTATE_ATTACK2] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			CEffect_Manager::GetInstance()->Play_SkillEffect("orbAttack", &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Attack2 > (*vecAnimFrame)[3])
		{
			bTimerAttack2 = false;
			m_fAnimationKeyFrameIndex_Attack2 = 0;

			m_IsSoundPlay[MonsterState::MSTATE_ATTACK2] = false;
			m_IsEffectPlay[MonsterState::MSTATE_ATTACK2] = false;

		}


	}

	if (bTimerHit)
	{
	
		//m_pPlayer->MoveForward(10.0f);
		m_fAnimationKeyFrameIndex_Hit += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (!m_IsSoundPlay[MonsterState::MSTATE_HIT] && m_fAnimationKeyFrameIndex_Hit > m_SoundFrame[MonsterState::MSTATE_HIT])
		{
			m_IsSoundPlay[MonsterState::MSTATE_HIT] = true;
			CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"NagaGuard_Hit_Sound");
		}


		if (!m_IsEffectPlay[MonsterState::MSTATE_HIT] && m_fAnimationKeyFrameIndex_Hit > m_EffectFrame[MonsterState::MSTATE_HIT])
		{
			m_IsEffectPlay[MonsterState::MSTATE_HIT] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			CEffect_Manager::GetInstance()->Play_SkillEffect("hh", &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Hit > (*vecAnimFrame)[MonsterState::MSTATE_HIT])
		{
			bTimerHit = false;
			m_fAnimationKeyFrameIndex_Hit = 0;

			m_IsSoundPlay[MonsterState::MSTATE_HIT] = false;
			m_IsEffectPlay[MonsterState::MSTATE_HIT] = false;

		}


	}

	if (bTimerDead == true)
	{
		

		//cout << m_fAnimationKeyFrameIndex_Dead << endl;
		if (m_bIsLife == true)
			m_fAnimationKeyFrameIndex_Dead += gt.DeltaTime() * 20;

		if (!m_IsSoundPlay[MonsterState::MSTATE_DEAD] && m_fAnimationKeyFrameIndex_Dead > m_SoundFrame[MonsterState::MSTATE_DEAD])
		{
			m_IsSoundPlay[MonsterState::MSTATE_DEAD] = true;
			CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"NagaGuard_Dead_Sound");
		}

		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (m_fAnimationKeyFrameIndex_Dead + 1 > (*vecAnimFrame)[MonsterState::MSTATE_DEAD])
		{
			m_bIsLife = false;
			bTimerDead = false;
			//m_fAnimationKeyFrameIndex_Dead = 0;
		}

	}
	

}

void AnimateStateMachine_NagaGuard::SetTimerTrueFalse()
{

	if (m_iAnimState == MonsterState::MSTATE_IDLE)
	{
		bTimerIdle = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex;
	}
	if (m_iAnimState == MonsterState::MSTATE_WALK)
	{
		bTimerWalk = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Walk;
	}
	if (m_iAnimState == MonsterState::MSTATE_ATTACK1)
	{
		bTimerAttack1 = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack1;
	}
	if (m_iAnimState == MonsterState::MSTATE_ATTACK2)
	{
		bTimerAttack2 = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;
	}
	
	if (m_iAnimState == MonsterState::MSTATE_HIT)
	{
		bTimerHit = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Hit;
	}

	if (m_iAnimState == MonsterState::MSTATE_DEAD)
	{
		bTimerDead = true;
		m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Dead;
	}

	


	
}

AnimateStateMachine_NagaGuard * AnimateStateMachine_NagaGuard::Create(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END])
{
	AnimateStateMachine_NagaGuard* pInstance = new AnimateStateMachine_NagaGuard(pObj, pMachineName, SoundFrame, EffectFrame);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"AnimateStateMachine_NagaGuard Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void AnimateStateMachine_NagaGuard::Free()
{
}
