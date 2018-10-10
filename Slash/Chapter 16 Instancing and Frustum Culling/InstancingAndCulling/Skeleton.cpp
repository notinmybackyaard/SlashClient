#include "stdafx.h"
#include "Skeleton.h"
#include "Component_Manager.h"
#include "DynamicMesh.h"
#include "Camera.h"
#include "Management.h"
#include "Renderer.h"
#include "Texture_Manager.h"
#include "Effect_Manager.h"
#include "Network.h"

CSkeleton::CSkeleton(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* meshName, bool isWarrior)
	: CGameObject(d3dDevice, srv, srvSize)
	, m_IsWarrior(isWarrior)
{
	m_pwstrMeshName = meshName;
}

CSkeleton::~CSkeleton()
{
}

void CSkeleton::OnPrepareRender()
{
}

void CSkeleton::Animate(const GameTimer & gt)
{
	AnimStateMachine->AnimationStateUpdate(gt);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&(GetWorld())));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

}

HRESULT CSkeleton::Initialize()
{
	//m_pMesh = dynamic_cast<DynamicMesh*>(CComponent_Manager::GetInstance()->Clone_Component(m_pwstrMeshName));
	string strTexName;
	wchar_t* machineName;
	if (m_IsWarrior)
	{
		m_pMesh = dynamic_cast<DynamicMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Warrior"));
		strTexName = "VillagerTex";
		machineName = L"Warrior";
	}

	else
	{
		m_pMesh = dynamic_cast<DynamicMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Mage"));
		strTexName = "MageTex";
		machineName = L"Mage";
	}

	if (nullptr == m_pMesh)
		return E_FAIL;


	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture(strTexName, CTexture_Manager::TEX_DEFAULT_2D);
	if (tex == nullptr)
		return E_FAIL;

	int test[State::STATE_END] = { 0, };
	AnimStateMachine = AnimateStateMachine_Skeleton::Create(this, machineName, test, test);
	if (AnimStateMachine == nullptr)
		return E_FAIL;

	AnimStateMachine->vecAnimFrame = &(dynamic_cast<DynamicMesh*>(m_pMesh)->vecAnimFrame);
	AnimStateMachine->SetAnimState(State::STATE_WALK);


	Mat = new Material;
	Mat->Name = "InsecMat";
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = tex->Num;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	XMStoreFloat4x4(&World, XMMatrixScaling(0.1f, 0.1f, 0.1f)*XMMatrixRotationX(1.7f)*XMMatrixRotationZ(3.14f)*XMMatrixTranslation(0.0f, 0.0f, 0.f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo_Head = dynamic_cast<DynamicMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Element_Head.IndexCount = Geo_Head->DrawArgs[" \"_head\""].IndexCount;
	Element_Head.StartIndexLocation = Geo_Head->DrawArgs[" \"_head\""].StartIndexLocation;
	Element_Head.BaseVertexLocation = Geo_Head->DrawArgs[" \"_head\""].BaseVertexLocation;
	Bounds = Geo_Head->DrawArgs[" \"_head\""].Bounds;


	Geo_Body = dynamic_cast<DynamicMesh*>(m_pMesh)->m_Geometry[1].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Element_Body.IndexCount = Geo_Body->DrawArgs[" \"_body\""].IndexCount;
	Element_Body.StartIndexLocation = Geo_Body->DrawArgs[" \"_body\""].StartIndexLocation;
	Element_Body.BaseVertexLocation = Geo_Body->DrawArgs[" \"_body\""].BaseVertexLocation;


	Geo_Right = dynamic_cast<DynamicMesh*>(m_pMesh)->m_Geometry[2].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Element_Right.IndexCount = Geo_Right->DrawArgs[" \"_rh-01\""].IndexCount;
	Element_Right.StartIndexLocation = Geo_Right->DrawArgs[" \"_rh-01\""].StartIndexLocation;
	Element_Right.BaseVertexLocation = Geo_Right->DrawArgs[" \"_rh-01\""].BaseVertexLocation;

	m_xmf3Scale = XMFLOAT3(0.05f, 0.05f, 0.05f);
	m_xmf3Rot = XMFLOAT3(1.7f, 0.f, 3.14f);


	//SetOOBB(XMFLOAT3(Bounds.Center.x * 0.05f, Bounds.Center.y * 0.05f, Bounds.Center.z * 0.05f), XMFLOAT3(Bounds.Extents.x * 0.05f, Bounds.Extents.y * 0.05f, Bounds.Extents.z * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


	return S_OK;
}

bool CSkeleton::Update(const GameTimer & gt)
{
	CGameObject::Update(gt);

	Animate(gt);


	m_pCamera = CManagement::GetInstance()->Get_MainCam();
	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();


	XMMATRIX world = XMLoadFloat4x4(&World);
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);


	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	// View space to the object's local space.
	XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

	// Transform the camera frustum from view space to the object's local space.
	BoundingFrustum localSpaceFrustum;
	mCamFrustum = *CManagement::GetInstance()->Get_CurScene()->Get_CamFrustum();
	mCamFrustum.Transform(localSpaceFrustum, viewToLocal);
	if ((localSpaceFrustum.Contains(Bounds /*m_xmOOBB*/) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
	{
		//cout << "보인당!" << endl;

		m_bIsVisiable = true;
		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
		objConstants.MaterialIndex = Mat->MatCBIndex;

		currObjectCB->CopyData(ObjCBIndex, objConstants);
	}
	else
	{
		//cout << "안보인당!" << endl;

		m_bIsVisiable = false;
	}

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
	objConstants.MaterialIndex = Mat->MatCBIndex;

	currObjectCB->CopyData(ObjCBIndex, objConstants);

	// Next FrameResource need to be updated too.


	///////////////////////////////////////////////////

	auto currMaterialCB = m_pFrameResource->MaterialCB.get();

	// Only update the cbuffer data if the constants have changed.  If the cbuffer
	// data changes, it needs to be updated for each FrameResource.


	XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

	MaterialConstants matConstants;
	matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;
	matConstants.FresnelR0 = Mat->FresnelR0;
	matConstants.Roughness = Mat->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;


	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);

	// Next FrameResource need to be updated too.
	//mat->NumFramesDirty--;

	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this); CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);

	return true;
}

void CSkeleton::Render(ID3D12GraphicsCommandList * cmdList)
{
	if (m_bIsVisiable && m_bIsConnected)
	{
		AnimStateMachine->SetTimerTrueFalse();


		Render_Head(cmdList);
		Render_Body(cmdList);
		Render_Right(cmdList);
	}

}
void CSkeleton::Render_Head(ID3D12GraphicsCommandList * cmdList)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo_Head->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Head->IndexBufferView());
	cmdList->IASetPrimitiveTopology(PrimitiveType);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	if (bIsUltimateState && m_IsWarrior)
	{
		Texture* WarriorUltimateTex = CTexture_Manager::GetInstance()->Find_Texture("WarriorUltimateTex", CTexture_Manager::TEX_DEFAULT_2D);
		if (WarriorUltimateTex == nullptr)
			return;
		tex.Offset(WarriorUltimateTex->Num, mCbvSrvDescriptorSize);
	}
	else
		tex.Offset(Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

	Mat->DiffuseSrvHeapIndex;
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ObjCBIndex * objCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + Mat->MatCBIndex*matCBByteSize;

	cmdList->SetGraphicsRootConstantBufferView(4, objCBAddress);
	//cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);
	cmdList->SetGraphicsRootShaderResourceView(5, matCBAddress);

	cmdList->SetGraphicsRootDescriptorTable(7, tex);

	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0][1];

	int iTest = AnimStateMachine->GetCurAnimFrame();
	int m_iCurAnimState = AnimStateMachine->GetAnimState();

	//cmdList->DrawIndexedInstanced(Element_Head.IndexCount, 1, Element_Head.StartIndexLocation, Element_Head.BaseVertexLocation , 0);
	//	dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0].
	cmdList->DrawIndexedInstanced(indexcnt, 1,
		dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[0][m_iCurAnimState],
		dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[0][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[0][m_iCurAnimState/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);
}

void CSkeleton::Render_Body(ID3D12GraphicsCommandList * cmdList)
{


	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo_Body->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Body->IndexBufferView());
	cmdList->IASetPrimitiveTopology(PrimitiveType);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	if (bIsUltimateState && m_IsWarrior)
	{
		Texture* WarriorUltimateTex = CTexture_Manager::GetInstance()->Find_Texture("WarriorUltimateTex", CTexture_Manager::TEX_DEFAULT_2D);
		if (WarriorUltimateTex == nullptr)
			return;
		tex.Offset(WarriorUltimateTex->Num, mCbvSrvDescriptorSize);
	}
	else
		tex.Offset(Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

	Mat->DiffuseSrvHeapIndex;
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ObjCBIndex * objCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + Mat->MatCBIndex*matCBByteSize;

	cmdList->SetGraphicsRootConstantBufferView(4, objCBAddress);
	//cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);
	cmdList->SetGraphicsRootShaderResourceView(5, matCBAddress);

	cmdList->SetGraphicsRootDescriptorTable(7, tex);

	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[1][1];

	int iTest = AnimStateMachine->GetCurAnimFrame();
	int m_iCurAnimState = AnimStateMachine->GetAnimState();


	cmdList->DrawIndexedInstanced(indexcnt, 1,
		Element_Body.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[1][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[1][m_iCurAnimState/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		Element_Body.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[1][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[1][m_iCurAnimState/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);

}


void CSkeleton::Render_Right(ID3D12GraphicsCommandList * cmdList)
{

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo_Right->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Right->IndexBufferView());
	cmdList->IASetPrimitiveTopology(PrimitiveType);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	if (bIsUltimateState && m_IsWarrior)
	{
		Texture* WarriorUltimateTex = CTexture_Manager::GetInstance()->Find_Texture("WarriorUltimateTex", CTexture_Manager::TEX_DEFAULT_2D);
		if (WarriorUltimateTex == nullptr)
			return;
		tex.Offset(WarriorUltimateTex->Num, mCbvSrvDescriptorSize);
	}
	else
		tex.Offset(Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

	Mat->DiffuseSrvHeapIndex;
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ObjCBIndex * objCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + Mat->MatCBIndex*matCBByteSize;

	cmdList->SetGraphicsRootConstantBufferView(4, objCBAddress);
	//cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);
	cmdList->SetGraphicsRootShaderResourceView(5, matCBAddress);

	cmdList->SetGraphicsRootDescriptorTable(7, tex);

	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[2][1];

	int iTest = AnimStateMachine->GetCurAnimFrame();
	int m_iCurAnimState = AnimStateMachine->GetAnimState();


	cmdList->DrawIndexedInstanced(indexcnt, 1,
		Element_Right.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[2][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[2][m_iCurAnimState/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		Element_Right.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[2][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[2][m_iCurAnimState/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);
}

void CSkeleton::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fRoll), XMConvertToRadians(fYaw));
	World = Matrix4x4::Multiply(mtxRotate, World);
}

CSkeleton * CSkeleton::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t* meshName, bool isWarrior)
{
	CSkeleton* pInstance = new CSkeleton(d3dDevice, srv, srvSize, meshName, isWarrior);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CSkeleton Created Failed");
		Safe_Release(pInstance);

	}

	return pInstance;
}

void CSkeleton::Move(const XMFLOAT3 & xmf3Shift, bool bVelocity)
{

	XMFLOAT3 CurPos = XMFLOAT3(World._41, World._42, World._43);

	m_xmf3Position = Vector3::Add(CurPos, xmf3Shift);

	XMFLOAT3 xmf3shiftTest = xmf3Shift;

	//XMFLOAT3 test = Vector3::Subtract(CurPos, xmf3shiftTest);

	//cout << xmf3Shift.x << "\t" << xmf3Shift.y << "\t" << xmf3Shift.z << endl;

	World._41 = m_xmf3Position.x;
	World._42 = m_xmf3Position.y;
	World._43 = m_xmf3Position.z;

}

void CSkeleton::SetUltimateEffect(bool isUltimate)
{
	if (!m_IsWarrior)
		return;

	dynamic_cast<AnimateStateMachine_Skeleton*>(AnimStateMachine)->SetUltimateEffect(isUltimate);
}

void CSkeleton::Free()
{
	CGameObject::Free();
}



// > ---------------------------- StateMachine_Player -----------------------------------

AnimateStateMachine_Skeleton::AnimateStateMachine_Skeleton(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END])
	: m_pMachineName(pMachineName)
	, m_pObject(pObj)
{
	for (int i = 0; i < State::STATE_END; ++i)
	{
		m_SoundFrame[i] = SoundFrame[i];
		m_EffectFrame[i] = EffectFrame[i];
		m_IsSoundPlay[i] = false;
		m_IsEffectPlay[i] = false;
	}
}

AnimateStateMachine_Skeleton::~AnimateStateMachine_Skeleton()
{
}

HRESULT AnimateStateMachine_Skeleton::Initialize()
{
	if (!wcscmp(m_pMachineName, L"Warrior"))
	{
		m_mapEffectName.emplace(State::STATE_ATTACK1, "Warrior_Turn");
		m_mapEffectName.emplace(State::STATE_ATTACK2, "Slash_00");
		m_mapEffectName.emplace(State::STATE_ATTACK3, "Drop");
		m_mapEffectName.emplace(State::STATE_ULTIMATE, "Trans_00");
	}
	else
	{
		m_mapEffectName.emplace(State::STATE_ATTACK1, "LightBall_00");
		m_mapEffectName.emplace(State::STATE_ATTACK2, "orbAttack");
		m_mapEffectName.emplace(State::STATE_ATTACK3, "Heal_00");
		m_mapEffectName.emplace(State::STATE_ULTIMATE, "Cast_00");
	}

	return S_OK;
}

void AnimateStateMachine_Skeleton::AnimationStateUpdate(const GameTimer & gt)
{
	if (bTimerIdle == true)
	{
		m_fAnimationKeyFrameIndex += gt.DeltaTime() * 25;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex;
		if (m_fAnimationKeyFrameIndex > (*vecAnimFrame)[0])
		{
			bTimerIdle = false;

			m_fAnimationKeyFrameIndex = 0;
		}
	}


	if (bTimerWalk == true)
	{
		m_fAnimationKeyFrameIndex_Walk += gt.DeltaTime() * 45;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Walk;
		if (m_fAnimationKeyFrameIndex_Walk > (*vecAnimFrame)[1])
		{
			bTimerWalk = false;
			m_fAnimationKeyFrameIndex_Walk = 0;
		}

	}


	if (bTimerAttack1 == true)
	{
		m_fAnimationKeyFrameIndex_Attack1 += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack1;
		if (!m_IsSoundPlay[State::STATE_ATTACK1] && m_fAnimationKeyFrameIndex_Attack1 > m_SoundFrame[State::STATE_ATTACK1])
		{
			m_IsSoundPlay[State::STATE_ATTACK1] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK1]);
		}

		if (!m_IsEffectPlay[State::STATE_ATTACK1] && m_fAnimationKeyFrameIndex_Attack1 > m_EffectFrame[State::STATE_ATTACK1])
		{
			m_IsEffectPlay[State::STATE_ATTACK1] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			//cout << "스킬!" << endl;

			CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK1], &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());
			//cout << "Player Pos : " << m_pObject->GetPosition().x << ", " << m_pObject->GetPosition().y << ", " << m_pObject->GetPosition().z << endl;
		}

		if (m_fAnimationKeyFrameIndex_Attack1 > (*vecAnimFrame)[2])
		{
			bTimerAttack1 = false;
			m_fAnimationKeyFrameIndex_Attack1 = 0.f;

			m_IsSoundPlay[State::STATE_ATTACK1] = false;
			m_IsEffectPlay[State::STATE_ATTACK1] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}

	}


	if (bTimerAttack2 == true)
	{

		m_fAnimationKeyFrameIndex_Attack2 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;

		if (!m_IsSoundPlay[State::STATE_ATTACK2] && m_fAnimationKeyFrameIndex_Attack2 > m_SoundFrame[State::STATE_ATTACK2])
		{
			m_IsSoundPlay[State::STATE_ATTACK2] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK2]);
		}

		if (!m_IsEffectPlay[State::STATE_ATTACK2] && m_fAnimationKeyFrameIndex_Attack2 > m_EffectFrame[State::STATE_ATTACK2])
		{
			m_IsEffectPlay[State::STATE_ATTACK2] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK2], &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());
		}

		if (m_fAnimationKeyFrameIndex_Attack2 > (*vecAnimFrame)[3])
		{
			bTimerAttack2 = false;
			m_fAnimationKeyFrameIndex_Attack2 = 0;

			m_IsSoundPlay[State::STATE_ATTACK2] = false;
			m_IsEffectPlay[State::STATE_ATTACK2] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}


	}

	if (bTimerAttack3 == true)
	{

		m_fAnimationKeyFrameIndex_Attack3 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (!m_IsSoundPlay[State::STATE_ATTACK3] && m_fAnimationKeyFrameIndex_Attack3 > m_SoundFrame[State::STATE_ATTACK3])
		{
			m_IsSoundPlay[State::STATE_ATTACK3] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK3]);		// > 모든 사운드가 들어갔을때 이렇게 바꿔야함!
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
		}

		if (!m_IsEffectPlay[State::STATE_ATTACK3] && m_fAnimationKeyFrameIndex_Attack3 > m_EffectFrame[State::STATE_ATTACK3])
		{
			m_IsEffectPlay[State::STATE_ATTACK3] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			if (dynamic_cast<CSkeleton*>(m_pObject)->GetIsWarrior())
				CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK3], &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());
			else
				CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK3], &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Attack3 > (*vecAnimFrame)[4])
		{
			bTimerAttack3 = false;
			m_fAnimationKeyFrameIndex_Attack3 = 0;

			m_IsSoundPlay[State::STATE_ATTACK3] = false;
			m_IsEffectPlay[State::STATE_ATTACK3] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}

	}

	if (bTimerDead == true)
	{
		//cout << m_fAnimationKeyFrameIndex_Dead << endl;
		if (m_bIsLife == true)
			m_fAnimationKeyFrameIndex_Dead += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (m_fAnimationKeyFrameIndex_Dead + 1> (*vecAnimFrame)[5])
		{
			m_bIsLife = false;
			bTimerDead = false;
			//m_fAnimationKeyFrameIndex_Dead = 0;
		}

	}

	if (bTimerUltimate == true) // 여기서부터 수정
	{

		m_fAnimationKeyFrameIndex_Ultimate += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;

		if (!m_IsSoundPlay[State::STATE_ULTIMATE] && m_fAnimationKeyFrameIndex_Ultimate > m_SoundFrame[State::STATE_ULTIMATE])
		{
			m_IsSoundPlay[State::STATE_ULTIMATE] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK2]);
		}

		if (!m_IsEffectPlay[State::STATE_ULTIMATE] && m_fAnimationKeyFrameIndex_Ultimate > m_EffectFrame[State::STATE_ULTIMATE])
		{
			m_IsEffectPlay[State::STATE_ULTIMATE] = true;
			CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ULTIMATE], &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Ultimate > (*vecAnimFrame)[State::STATE_ULTIMATE])
		{
			bTimerUltimate = false;
			m_fAnimationKeyFrameIndex_Ultimate = 0;

			m_IsSoundPlay[State::STATE_ULTIMATE] = false;
			m_IsEffectPlay[State::STATE_ULTIMATE] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}

	}

	if (bTimerRoll == true)
	{

		m_fAnimationKeyFrameIndex_Roll += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (!m_IsSoundPlay[State::STATE_ROLL] && m_fAnimationKeyFrameIndex_Roll > m_SoundFrame[State::STATE_ROLL])
		{
			m_IsSoundPlay[State::STATE_ROLL] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK3]);		// > 모든 사운드가 들어갔을때 이렇게 바꿔야함!
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
		}

		if (!m_IsEffectPlay[State::STATE_ROLL] && m_fAnimationKeyFrameIndex_Roll > m_EffectFrame[State::STATE_ROLL])
		{
			m_IsEffectPlay[State::STATE_ROLL] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			CEffect_Manager::GetInstance()->Play_SkillEffect("Roll_00", &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());
		}

		if (m_fAnimationKeyFrameIndex_Roll > (*vecAnimFrame)[State::STATE_ROLL])
		{
			bTimerRoll = false;
			m_fAnimationKeyFrameIndex_Roll = 0;

			m_IsSoundPlay[State::STATE_ROLL] = false;
			m_IsEffectPlay[State::STATE_ROLL] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}


	}

	if (bTimerHit == true)
	{

		m_fAnimationKeyFrameIndex_Hit += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (!m_IsSoundPlay[State::STATE_HIT] && m_fAnimationKeyFrameIndex_Hit > m_SoundFrame[State::STATE_HIT])
		{
			m_IsSoundPlay[State::STATE_HIT] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[State::STATE_ATTACK3]);		// > 모든 사운드가 들어갔을때 이렇게 바꿔야함!
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
		}

		if (!m_IsEffectPlay[State::STATE_HIT] && m_fAnimationKeyFrameIndex_Hit > m_EffectFrame[State::STATE_HIT])
		{
			m_IsEffectPlay[State::STATE_HIT] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			CEffect_Manager::GetInstance()->Play_SkillEffect("hh", &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Hit > (*vecAnimFrame)[State::STATE_HIT])
		{
			bTimerHit = false;
			m_fAnimationKeyFrameIndex_Hit = 0;

			m_IsSoundPlay[State::STATE_HIT] = false;
			m_IsEffectPlay[State::STATE_HIT] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}


	}
}

void AnimateStateMachine_Skeleton::SetUltimateEffect(bool isUltimate)
{
		m_mapEffectName.clear();
		if (!isUltimate)
		{
			m_mapEffectName.emplace(State::STATE_ATTACK1, "Warrior_Turn");
			m_mapEffectName.emplace(State::STATE_ATTACK2, "Slash_00");
			m_mapEffectName.emplace(State::STATE_ATTACK3, "Drop");
			m_mapEffectName.emplace(State::STATE_ULTIMATE, "Trans_00");
		}
		else
		{
			m_mapEffectName.emplace(State::STATE_ATTACK1, "Ultimate1");
			m_mapEffectName.emplace(State::STATE_ATTACK2, "UtimateAttack_2");
			m_mapEffectName.emplace(State::STATE_ATTACK3, "Ax_00");
			m_mapEffectName.emplace(State::STATE_ULTIMATE, "Trans_00");
		}

}

AnimateStateMachine_Skeleton * AnimateStateMachine_Skeleton::Create(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END])
{
	AnimateStateMachine_Skeleton* pInstance = new AnimateStateMachine_Skeleton(pObj, pMachineName, SoundFrame, EffectFrame);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"AnimateStateMachine_Skeleton Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void AnimateStateMachine_Skeleton::Free()
{
}
