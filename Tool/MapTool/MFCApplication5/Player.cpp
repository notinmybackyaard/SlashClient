#include "stdafx.h"
#include "Player.h"
#include "Define.h"
#include "DynamicMesh.h"
#include "InputDevice.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "ObjectManager.h"
#include "Renderer.h"

Player::Player(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize)
	: CGameObject(d3dDevice, srv, srvSize)
{
	m_preKeyInputTime = 0;
	m_curKeyInputTime = 0;

}

Player::~Player()
{
	delete m_pMesh;

	delete Mat;

	//여기
}


void Player::Animate(const GameTimer & gt)
{
	if (AnimStateMachine->GetAnimState() == AnimStateMachine->Attack1State)
	{
		if (GetAnimateMachine()->GetCurAnimFrame() == 0)
		{
			if (m_bAttackMotionForSound == true)
			{
				//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
				m_bAttackMotionForSound = false;
			}
		}
		if (GetAnimateMachine()->GetCurAnimFrame() != 0)
		{
			m_bAttackMotionForSound = true;
		}
// > =======================Test===========================
		//if (GetAnimateMachine()->GetCurAnimFrame() == 8)
		//{
		//	AnimStateMachine->SetAnimState(AnimStateMachine->IdleState);
		//	CNetwork::GetInstance()->SendStopPacket();
		//}
// > ===========================================================

	}
	//if (GetHp() < 0)
	//{
	//	SetObjectAnimState(AnimStateMachine->DeadState);
	//	AnimStateMachine->AnimationStateUpdate(gt);
	//	return;
	//}



	AnimStateMachine->AnimationStateUpdate(gt); //애니메이션 상태 설정해주는 함수

	KeyInput(gt);

	//if (CInputDevice::GetInstance()->AnyKeyInput())
	//{
	//	
	//	if (!AnimStateMachine->bTimerAttack1 &&
	//		!AnimStateMachine->bTimerAttack2 &&
	//		!AnimStateMachine->bTimerAttack3 /*&&
	//		!pTestMesh->bTimerTestWalk*/
	//		)
	//	{
	//		
	//		AnimStateMachine->SetAnimState(AnimStateMachine->IdleState);

	//	//	KeyInputTest = 0;

	//	}
	//}

	// 받을 때 갱신해야함 // 월드 행렬로..? // 서버작업 // 회전 받을때랑 // 위치 받을때 // 아도가 그냥 대충 하자고함
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&(GetWorld())));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

}

bool Player::Update(const GameTimer & gt)
{
	//return true;
	CGameObject::Update(gt);

	Animate(gt);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();


	XMMATRIX world = XMLoadFloat4x4(&World);
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);

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
	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);

	return true;
}

void Player::Render(ID3D12GraphicsCommandList * cmdList)
{
	//return;
	AnimStateMachine->SetTimerTrueFalse(); //어떤 애니메이션을 동작 시켜주는 지 

	Render_Head(cmdList);
	Render_Body(cmdList);
	Render_Right(cmdList);
	//Render_Left(cmdList);
}
HRESULT Player::Initialize()
{
	m_pMesh = dynamic_cast<DynamicMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Warrior"));

	if (nullptr == m_pMesh)
		return E_FAIL;
	

	// > 테스트용으로 넣어둠
	int test[AnimateStateMachine::STATE_END] = { 0, };
	AnimStateMachine = AnimateStateMachine_Player::Create(this, L"Warrior", test, test);
	if (AnimStateMachine == nullptr)
		return E_FAIL;

	AnimStateMachine->vecAnimFrame = &(dynamic_cast<DynamicMesh*>(m_pMesh)->vecAnimFrame);

	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture(L"VillagerTex", HEAP_DEFAULT);			// 이런식으로 가져옴
	if (tex == nullptr)
		return E_FAIL;

	Mat = new Material;
	Mat->Name = "InsecMat";
	Mat->MatCBIndex = 0;
	Mat->DiffuseSrvHeapIndex = tex->Num;			// 텍스쳐 힙에 저장ㄷ외나 수아아ㅇ아ㅏ 아하 지희야그럼 이거이렇게바꿔야함 
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	XMStoreFloat4x4(&World, XMMatrixScaling(0.05f, 0.05f, 0.05f)*XMMatrixRotationX(1.7f)*XMMatrixRotationZ(3.14f)*XMMatrixTranslation(0.0f, 0.0f, 0.0f));

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


	//m_xmf3Scale = XMFLOAT3(0.05f, 0.05f, 0.05f);
	//m_xmf3Rot = XMFLOAT3(1.7f, 0.f, 3.14f);

	//SetOOBB(XMFLOAT3(Bounds.Center.x * 0.05f, Bounds.Center.y * 0.05f, Bounds.Center.z * 0.05f), XMFLOAT3(Bounds.Extents.x * 0.05f, Bounds.Extents.y * 0.05f, Bounds.Extents.z * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	//Geo_Left = dynamic_cast<DynamicMesh*>(m_pMesh)->m_Geometry[3].get();
	//PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//Element_Left.IndexCount = Geo_Left->DrawArgs["mage_Left"].IndexCount;
	//Element_Left.StartIndexLocation = Geo_Left->DrawArgs["mage_Left"].StartIndexLocation;
	//Element_Left.BaseVertexLocation = Geo_Left->DrawArgs["mage_Left"].BaseVertexLocation;

	//m_pBoundMesh = dynam


	//m_pBoundMesh = CBoundingBox::Create(m_d3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize, GetPosition(), XMFLOAT3(0.5f, 0.5f, 0.5f));
	return S_OK;
}
void Player::Free()
{
	CGameObject::Free();
}
void Player::Render_Head(ID3D12GraphicsCommandList * cmdList)
{
	XMFLOAT3 look = XMFLOAT3(-World._21, -World._22, -World._23);

	Vector3::Normalize(look);



	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo_Head->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Head->IndexBufferView());
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

	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0][1];

	int iTest = AnimStateMachine->GetCurAnimFrame();
	int KeyInputTest = AnimStateMachine->GetAnimState();


	//cmdList->DrawIndexedInstanced(Element_Head.IndexCount, 1, Element_Head.StartIndexLocation, Element_Head.BaseVertexLocation , 0);
	//	dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0].
	cmdList->DrawIndexedInstanced(indexcnt, 1,
		dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[0][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[0][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[0][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);
}
void Player::Render_Body(ID3D12GraphicsCommandList * cmdList)
{


	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo_Body->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Body->IndexBufferView());
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

	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[1][1];

	int iTest = AnimStateMachine->GetCurAnimFrame();
	int KeyInputTest = AnimStateMachine->GetAnimState();

	cmdList->DrawIndexedInstanced(indexcnt, 1,
		Element_Body.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[1][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[1][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		Element_Body.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[1][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[1][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);

}
void Player::Render_Right(ID3D12GraphicsCommandList * cmdList)
{

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo_Right->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Right->IndexBufferView());
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

	auto indexcnt = dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[2][1];

	int iTest = AnimStateMachine->GetCurAnimFrame();
	int KeyInputTest = AnimStateMachine->GetAnimState();


	cmdList->DrawIndexedInstanced(indexcnt, 1,
		Element_Right.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[2][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[2][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		Element_Right.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[2][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[2][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);
}
Player * Player::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
{
	Player* pInstance = new Player(d3dDevice, srv, srvSize);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Player Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

//void Player::Render_Left(ID3D12GraphicsCommandList * cmdList)
//{
//	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
//	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
//
//	auto objectCB = m_pFrameResource->ObjectCB->Resource();
//	auto matCB = m_pFrameResource->MaterialCB->Resource();
//
//	cmdList->IASetVertexBuffers(0, 1, &Geo_Left->VertexBufferView());
//	cmdList->IASetIndexBuffer(&Geo_Left->IndexBufferView());
//	cmdList->IASetPrimitiveTopology(PrimitiveType);
//
//	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
//	tex.Offset(Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);
//
//	Mat->DiffuseSrvHeapIndex;
//	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ObjCBIndex * objCBByteSize;
//	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + Mat->MatCBIndex*matCBByteSize;
//
//	cmdList->SetGraphicsRootConstantBufferView(4, objCBAddress);
//	//cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);
//	cmdList->SetGraphicsRootShaderResourceView(5, matCBAddress);
//
//	cmdList->SetGraphicsRootDescriptorTable(7, tex);
//
//
//	cmdList->DrawIndexedInstanced(Element_Right.IndexCount, 1, 
//		Element_Right.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset_Left[iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset_Left[KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
//		Element_Right.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset_Left[iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset_Left[KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
//		0);
//
//
//	//cmdList->DrawIndexedInstanced(Element_Left.IndexCount, 1, Element_Left.StartIndexLocation, Element_Left.BaseVertexLocation, 0);
//}


void Player::SetPosition(float x, float y, float z)
{
	World._41 = x;
	World._42 = y;
	World._43 = z;

	//cout << World._11 << " " << World._12 << " " << World._13 << " " << World._14 << endl;
	//cout << World._21 << " " << World._22 << " " << World._23 << " " << World._24 << endl;
	//cout << World._31 << " " << World._32 << " " << World._33 << " " << World._34 << endl;
	//cout << World._41 << " " << World._42 << " " << World._43 << " " << World._44 << endl << endl;
}

void Player::SetPosition(XMFLOAT3 xmf3Position)
{
	Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z));
}

void Player::MoveStrafe(float fDistance)
{
}

void Player::MoveUp(float fDistance)
{
}

void Player::MoveForward(float fDistance)
{
}

void Player::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fRoll), XMConvertToRadians(fYaw));
	World = Matrix4x4::Multiply(mtxRotate, World);
}

void Player::Rotate(XMFLOAT3 * pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	World = Matrix4x4::Multiply(mtxRotate, World);
}


void Player::Move(const XMFLOAT3 & xmf3Shift, bool bVelocity)
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

void Player::KeyInput(const GameTimer & gt)
{

}

//void Player::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, const GameTimer & gt)
//{
//	
//
//	
//		if (dwDirection)
//		{
//			XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
//			if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Height, fDistance);
//			if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Height, -fDistance);
//			if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Width, fDistance);
//			if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Width, -fDistance);
//
//			//	xmf3Shift = Vector3::Normalize(xmf3Shift);
//			//xmf3Shift += m_MovingRefletVector;
//			//xmf3Shift = Vector3::Normalize(xmf3Shift);
//
//			// S = xmf3Shift
//			// n = m_MovingRfelctVector
//			// P = xmf3Shift
//
//			// S = P - n(P·n)
//
//			/*cout << " ---------------------" << endl;
//			cout << "Shift : " << xmf3Shift.x << "\t" << xmf3Shift.y << "\t" << xmf3Shift.z << endl;
//			cout << "m_MovingRefletVector : " << m_MovingRefletVector.x << "\t" << m_MovingRefletVector.y << "\t" << m_MovingRefletVector.z << endl;
//			cout << " ---------------------" << endl;*/
//
//			float cosCeta = Vector3::DotProduct(m_MovingRefletVector, xmf3Shift);
//			if (cosCeta < 0)
//				//if(!Vector3::IsEqual(Vector3::Normalize(m_MovingRefletVector), Vector3::Normalize(xmf3Shift)))
//				xmf3Shift = Vector3::Subtract(xmf3Shift, Vector3::MultiplyScalr(m_MovingRefletVector, Vector3::DotProduct(xmf3Shift, m_MovingRefletVector)));
//
//			//cout << xmf3Shift.x << "\t" << xmf3Shift.y << "\t" << xmf3Shift.z << endl;
//			//cout << "m_MovingRefletVector : " << m_MovingRefletVector.x << "\t" << m_MovingRefletVector.y << "\t" << m_MovingRefletVector.z << endl;
//
//		
//
//			Move(xmf3Shift, bUpdateVelocity);
//
//
//		}
//
//	
//}


//--------------------------------------- AnimateStateMachine-----------------------------------------

AnimateStateMachine_Player::AnimateStateMachine_Player(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[AnimateStateMachine::STATE_END], int EffectFrame[AnimateStateMachine::STATE_END])
	: m_pMachineName(pMachineName)
	, m_pObject(pObj)
{
	for (int i = 0; i < AnimateStateMachine::STATE_END; ++i)
	{
		m_SoundFrame[i] = SoundFrame[i];
		m_EffectFrame[i] = EffectFrame[i];
		m_IsSoundPlay[i] = false;
		m_IsEffectPlay[i] = false;
	}
}

AnimateStateMachine_Player::~AnimateStateMachine_Player()
{
}

HRESULT AnimateStateMachine_Player::Initialize()
{
	return S_OK;
}

// > ---------------------------- StateMachine_Player -----------------------------------
void AnimateStateMachine_Player::AnimationStateUpdate(const GameTimer & gt)
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
		m_fAnimationKeyFrameIndex_Attack1 += gt.DeltaTime() *20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack1;
		if (!m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK1] && m_fAnimationKeyFrameIndex_Attack1 > m_SoundFrame[AnimateStateMachine::STATE_ATTACK1])
		{
			m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK1] = true;
			//CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
			//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[AnimateStateMachine::STATE_ATTACK1]);
		}

		if (!m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK1] && m_fAnimationKeyFrameIndex_Attack1 > m_EffectFrame[AnimateStateMachine::STATE_ATTACK1])
		{
			m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK1] = true;
			// > 스킬넣어주기
			//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
			cout << "스킬!" << endl;
			//CEffect_Manager::GetInstance()->Play_SkillEffect("Warrior_Turn", &m_pObject->GetWorld());
			cout << "Player Pos : " << m_pObject->GetPosition().x << ", " << m_pObject->GetPosition().y << ", " << m_pObject->GetPosition().z << endl;
		}

		if (m_fAnimationKeyFrameIndex_Attack1 > (*vecAnimFrame)[2])
		{
			bTimerAttack1 = false;
			m_fAnimationKeyFrameIndex_Attack1 = 0.f;

			m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK1] = false;
			m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK1] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
		}

	}


	if (bTimerAttack2 == true)
	{

		m_fAnimationKeyFrameIndex_Attack2 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack2;

		//if (!m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK2] && m_fAnimationKeyFrameIndex_Attack1 > m_SoundFrame[AnimateStateMachine::STATE_ATTACK2])
		//{
		//	m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK2] = true;
		////	CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
		//	//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[AnimateStateMachine::STATE_ATTACK2]);
		//}

		//if (!m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK2] && m_fAnimationKeyFrameIndex_Attack1 > m_EffectFrame[AnimateStateMachine::STATE_ATTACK2])
		//{
		//	m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK2] = true;
		//	// > 스킬넣어주기
		//	//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
		//	CEffect_Manager::GetInstance()->Play_SkillEffect("hh");
		//}

		if (m_fAnimationKeyFrameIndex_Attack2 > (*vecAnimFrame)[3])
		{
			bTimerAttack2 = false;
			m_fAnimationKeyFrameIndex_Attack2 = 0;

			m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK2] = false;
			m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK2] = false;
		}


	}



	if (bTimerAttack3 == true)
	{

		m_fAnimationKeyFrameIndex_Attack3 += gt.DeltaTime() * 30;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		//if (!m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK3] && m_fAnimationKeyFrameIndex_Attack1 > m_SoundFrame[AnimateStateMachine::STATE_ATTACK3])
		//{
		//	m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK3] = true;
		//	//CManagement::GetInstance()->GetSound()->PlayEffect(m_pMachineName, m_pStateName[AnimateStateMachine::STATE_ATTACK3]);		// > 모든 사운드가 들어갔을때 이렇게 바꿔야함!
		//	CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
		//}

		//if (!m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK3] && m_fAnimationKeyFrameIndex_Attack1 > m_EffectFrame[AnimateStateMachine::STATE_ATTACK3])
		//{
		//	m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK3] = true;
		//	// > 스킬넣어주기
		//	//CEffect_Manager::GetInstance()->Play_SkillEffect("스킬이름");
		//	CEffect_Manager::GetInstance()->Play_SkillEffect("hh", &m_pObject->GetWorld());
		//}

		if (m_fAnimationKeyFrameIndex_Attack3 > (*vecAnimFrame)[4])
		{
			bTimerAttack3 = false;
			m_fAnimationKeyFrameIndex_Attack3 = 0;

			m_IsSoundPlay[AnimateStateMachine::STATE_ATTACK3] = false;
			m_IsEffectPlay[AnimateStateMachine::STATE_ATTACK3] = false;
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




}

AnimateStateMachine_Player * AnimateStateMachine_Player::Create(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[AnimateStateMachine::STATE_END], int EffectFrame[AnimateStateMachine::STATE_END])
{
	AnimateStateMachine_Player* pInstance = new AnimateStateMachine_Player(pObj, pMachineName, SoundFrame, EffectFrame);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"AnimateStateMachine_Player Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void AnimateStateMachine_Player::Free()
{
}
