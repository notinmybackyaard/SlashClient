#include "stdafx.h"
#include "SelectPlayer.h"
#include "Define.h"
#include "DynamicMesh.h"
#include "InputDevice.h"
#include "Network.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Effect_Manager.h"
#include "Management.h"
#include "Renderer.h"
#include "Camera.h"
#include "HPBar.h"
#include "NumUI.h"
#include "Protocol.h"

SelectPlayer::SelectPlayer(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, bool isWarrior)
	: CGameObject(d3dDevice, srv, srvSize)
	, m_IsWarrior(isWarrior)
{
	m_preKeyInputTime = 0;
	m_curKeyInputTime = 0;
}

SelectPlayer::~SelectPlayer()
{
	delete m_pMesh;

	delete Mat;

	//여기
}


void SelectPlayer::Animate(const GameTimer & gt)
{
	if (AnimStateMachine->GetAnimState() == State::STATE_ATTACK1)
	{
		if (GetAnimateMachine()->GetCurAnimFrame() == 0)
		{
			if (m_bAttackMotionForSound == true)
			{
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Attack");
				m_bAttackMotionForSound = false;
			}
		}
		if (GetAnimateMachine()->GetCurAnimFrame() != 0)
		{
			m_bAttackMotionForSound = true;
		}
	}
	//if (GetHp() < 0)
	//{
	//	SetObjectAnimState(State::STATE_DEAD);
	//	AnimStateMachine->AnimationStateUpdate(gt);
	//	return;
	//}


	AnimStateMachine->AnimationStateUpdate(gt); //애니메이션 상태 설정해주는 함수

	//KeyInput(gt);

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
	//		AnimStateMachine->SetAnimState(State::STATE_IDLE);

	//	//	KeyInputTest = 0;

	//	}
	//}

	// 받을 때 갱신해야함 // 월드 행렬로..? // 서버작업 // 회전 받을때랑 // 위치 받을때 // 아도가 그냥 대충 하자고함
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&(GetWorld())));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

}

bool SelectPlayer::Update(const GameTimer & gt)
{
	if (!m_IsEnable)
		return false;
	CheckUltimate(gt);

	//return true;
	CGameObject::Update(gt);

	Animate(gt);
	//m_HPBar->SetHp(m_pSelectPlayer->GetHp());
	//cout << "SelectPlayer HP : " << GetHp() << endl;
	//cout << "HP Bar : " << m_HpBar->GetHp() << endl;
	if (!IsSoundIn)
	{
		float X = World._41;
		float Z = World._43;
		if (X > 120.f && X < 160.f)
		{
			if (Z > 45.f && Z < 90.f)
			{
				IsSoundIn = true;
				//
			}
		}
	}

	else
	{
		if (!IsSoundChange)
		{
			CManagement::GetInstance()->GetSound()->Set_BGM_Volume(-1.f * gt.DeltaTime() * 5.f);
			float fVolume = CManagement::GetInstance()->GetSound()->Get_BGM_Volume();
			if (fVolume < 0.2)
			{
				CManagement::GetInstance()->GetSound()->PlayBGM(L"Sound", L"Combat", 1.f);
				//CManagement::GetInstance()->GetSound()->Set_BGM_Volume(-1.f * gt.DeltaTime() * 30.f);
				IsSoundChange = true;
			}
		}
	}

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

	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);

	return true;
}

void SelectPlayer::Render(ID3D12GraphicsCommandList * cmdList)
{
	//return;
	AnimStateMachine->SetTimerTrueFalse(); //어떤 애니메이션을 동작 시켜주는 지 

	Render_Head(cmdList);
	Render_Body(cmdList);
	Render_Right(cmdList);
	//Render_Left(cmdList);
}

HRESULT SelectPlayer::Initialize()
{
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

	// > 테스트용으로 넣어둠
	int test[State::STATE_END] = { 0, };
	AnimStateMachine = AnimateStateMachine_SelectPlayer::Create(this, machineName, test, test);
	if (AnimStateMachine == nullptr)
		return E_FAIL;


	AnimStateMachine->vecAnimFrame = &(dynamic_cast<DynamicMesh*>(m_pMesh)->vecAnimFrame);

	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture(strTexName, CTexture_Manager::TEX_DEFAULT_2D);			// 이런식으로 가져옴
	if (tex == nullptr)
		return E_FAIL;

	Mat = new Material;
	Mat->Name = "InsecMat";
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = tex->Num;			// 텍스쳐 힙에 저장ㄷ외나 수아아ㅇ아ㅏ 아하 지희야그럼 이거이렇게바꿔야함 
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	XMStoreFloat4x4(&World, XMMatrixScaling(0.07f, 0.07f, 0.07f)*XMMatrixRotationX(1.7f)*XMMatrixRotationZ(3.14f)*XMMatrixTranslation(0.0f, 0.0f, 0.0f));

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


	// > Hp, Exp, Gage Bar UI Setting
	XMFLOAT2 move = XMFLOAT2(-0.3f, 7.3f);

	move.x = 0.f;
	move.y = -11.16980f;

	XMFLOAT2 scale = XMFLOAT2(1.2f, 0.125f);
	scale.x = 5.f;
	scale.y = 0.049f;
	float size = 0.695f;

	return S_OK;
}
void SelectPlayer::Free()
{
	CGameObject::Free();
}
void SelectPlayer::Render_Head(ID3D12GraphicsCommandList * cmdList)
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
	int KeyInputTest = AnimStateMachine->GetAnimState();


	//cmdList->DrawIndexedInstanced(Element_Head.IndexCount, 1, Element_Head.StartIndexLocation, Element_Head.BaseVertexLocation , 0);
	//	dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0].
	cmdList->DrawIndexedInstanced(indexcnt, 1,
		dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[0][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[0][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[0][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[0][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);
}

void SelectPlayer::Render_Body(ID3D12GraphicsCommandList * cmdList)
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
	int KeyInputTest = AnimStateMachine->GetAnimState();

	cmdList->DrawIndexedInstanced(indexcnt, 1,
		Element_Body.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[1][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[1][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		Element_Body.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[1][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[1][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);

}
void SelectPlayer::Render_Right(ID3D12GraphicsCommandList * cmdList)
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
	int KeyInputTest = AnimStateMachine->GetAnimState();


	cmdList->DrawIndexedInstanced(indexcnt, 1,
		Element_Right.StartIndexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexOffset[2][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecIndexAnimOffset[2][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		Element_Right.BaseVertexLocation + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexOffset[2][iTest] + dynamic_cast<DynamicMesh*>(m_pMesh)->m_vecVertexAnimOffset[2][KeyInputTest/*dynamic_cast<DynamicMesh*>(m_pMesh)->iAnimframe*/],
		0);
}
SelectPlayer * SelectPlayer::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, bool isWarrior)
{
	SelectPlayer* pInstance = new SelectPlayer(d3dDevice, srv, srvSize, isWarrior);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"SelectPlayer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SelectPlayer::SetUltimateEffect(bool isUltimate)
{
	if (!m_IsWarrior)
		return;

	dynamic_cast<AnimateStateMachine_SelectPlayer*>(AnimStateMachine)->SetUltimateEffect(isUltimate);
}

void SelectPlayer::CheckUltimate(const GameTimer & gt)
{
	if (bIsUltimateState)
	{
		m_fUltimateTime -= gt.DeltaTime();
		if (m_fUltimateTime < 0.0f)
		{
			m_fUltimateTime = 20.0f;
			bIsUltimateState = false;
			SetUltimateEffect(false);
			CNetwork::GetInstance()->SendUltimateOffPacket();
		}
	}
}

void SelectPlayer::SetPosition(XMFLOAT3 xmf3Position)
{
	Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z));
}

void SelectPlayer::MoveStrafe(float fDistance)
{
}

void SelectPlayer::MoveUp(float fDistance)
{
}

void SelectPlayer::MoveForward(float fDistance)
{
}

void SelectPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fRoll), XMConvertToRadians(fYaw));
	World = Matrix4x4::Multiply(mtxRotate, World);
}

void SelectPlayer::Rotate(XMFLOAT3 * pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	World = Matrix4x4::Multiply(mtxRotate, World);
}


void SelectPlayer::Move(const XMFLOAT3 & xmf3Shift, bool bVelocity)
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

void SelectPlayer::KeyInput(const GameTimer & gt)
{
	if (GetHp() <= 0) return;
	if ((AnimStateMachine->GetAnimState() == State::STATE_IDLE ||
		AnimStateMachine->GetAnimState() == State::STATE_WALK) == false)
		return;

	if (CManagement::GetInstance()->Get_IsStop() == true)
		return;

	DWORD dwDirection = 0;
	static bool IsSelectPlayerMoved = false;


	if (CManagement::GetInstance()->Get_MainCam() != NULL)
	{
		if (KeyBoard_Input(DIK_UP) == CInputDevice::INPUT_PRESS) dwDirection |= CS_DIR_FORWARD;
		if (KeyBoard_Input(DIK_DOWN) == CInputDevice::INPUT_PRESS) dwDirection |= CS_DIR_BACKWARD;
		if (KeyBoard_Input(DIK_LEFT) == CInputDevice::INPUT_PRESS) dwDirection |= CS_DIR_LEFT;
		if (KeyBoard_Input(DIK_RIGHT) == CInputDevice::INPUT_PRESS) dwDirection |= CS_DIR_RIGHT;
		if (KeyBoard_Input(DIK_LSHIFT) == CInputDevice::INPUT_DOWN) dwDirection |= CS_ROLL; // 이걸 따로 함수로 만들자
	}

	//m_curKeyInputTime = gt.TotalTime();
	//if (m_curKeyInputTime - m_preKeyInputTime > gt.DeltaTime())
	{
		if (0 == dwDirection)
		{
			if (IsSelectPlayerMoved)
			{
				if (AnimStateMachine->GetAnimState() != State::STATE_ROLL)
				{
					AnimStateMachine->SetAnimState(STATE_IDLE);
					CNetwork::GetInstance()->SendStopPacket();
					IsSelectPlayerMoved = false;
				}
			}
		}
		else
		{
			CNetwork::GetInstance()->SendDirKeyPacket(dwDirection);
			IsSelectPlayerMoved = true;
		}
		//m_preKeyInputTime = gt.TotalTime();
	}
	if (m_bIsConnected)
	{
		auto m_pCamera = CManagement::GetInstance()->Get_MainCam();
		if (KeyBoard_Input(DIK_1) == CInputDevice::INPUT_DOWN)
		{
			if (!m_IsWarrior)//법사일때 이펙트 사운드
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Mage_Attack1_Sound");
			else
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Warrior_Attack1_Sound");

			m_pCamera->SetCameraEffect(Camera::SHAKING);
			CNetwork::GetInstance()->SendAttack1Packet();
		}
		else if (KeyBoard_Input(DIK_2) == CInputDevice::INPUT_DOWN)
		{
			if (!m_IsWarrior)//법사일때 이펙트 사운드
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Mage_Attack2_Sound");
			else
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Warrior_Attack2_Sound");


			CNetwork::GetInstance()->SendAttack2Packet();

		}
		else if (KeyBoard_Input(DIK_3) == CInputDevice::INPUT_DOWN)
		{
			if (!m_IsWarrior)//법사일때 이펙트 사운드
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Mage_Attack3_Sound");
			else
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Warrior_Attack3_Sound");

			CNetwork::GetInstance()->SendAttack3Packet();
		}
		else if (KeyBoard_Input(DIK_R) == CInputDevice::INPUT_DOWN)
		{
			if (bIsUltimateState) return;

			if (!m_IsWarrior)//법사일때 이펙트 사운드
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Mage_Ultimate_Sound");
			else
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Warrior_Ultimate_Sound");



			m_pCamera->SetCameraEffect(Camera::ZOOMINROUNDULTIMATE, CManagement::GetInstance()->Find_Object(L"Layer_SelectPlayer"));
			SetObjectAnimState(State::STATE_ULTIMATE);
			CNetwork::GetInstance()->SendUltimateStartPacket();

		}

		else if (KeyBoard_Input(DIK_LSHIFT) == CInputDevice::INPUT_DOWN)
		{
			if (!m_IsWarrior)//법사일때 이펙트 사운드
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Mage_Roll_Sound");
			else
				CManagement::GetInstance()->GetSound()->PlayEffect(L"Sound", L"Warrior_Roll_Sound");

		}

	}
}

//--------------------------------------- AnimateStateMachine-----------------------------------------

AnimateStateMachine_SelectPlayer::AnimateStateMachine_SelectPlayer(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END])
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

AnimateStateMachine_SelectPlayer::~AnimateStateMachine_SelectPlayer()
{
}

HRESULT AnimateStateMachine_SelectPlayer::Initialize()
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

// > ---------------------------- StateMachine_SelectPlayer -----------------------------------
void AnimateStateMachine_SelectPlayer::AnimationStateUpdate(const GameTimer & gt)
{
	if (bTimerIdle == true)
	{
		m_fAnimationKeyFrameIndex += gt.DeltaTime() * 25;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex;
		if (m_fAnimationKeyFrameIndex > (*vecAnimFrame)[State::STATE_IDLE])
		{
			bTimerIdle = false;

			m_fAnimationKeyFrameIndex = 0;
		}
	}


	if (bTimerWalk == true)
	{
		m_fAnimationKeyFrameIndex_Walk += gt.DeltaTime() * 45;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Walk;
		if (m_fAnimationKeyFrameIndex_Walk > (*vecAnimFrame)[State::STATE_WALK])
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
			//cout << "스킬!" << endl;			XMFLOAT4X4 matWorld = Matrix4x4::Identity();
			XMFLOAT4X4 matWorld = Matrix4x4::Identity();
			matWorld._41 = m_pObject->GetWorld()._41;
			matWorld._42 = m_pObject->GetWorld()._42;
			matWorld._43 = m_pObject->GetWorld()._43;
			//CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK1], &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());
		}

		if (m_fAnimationKeyFrameIndex_Attack1 > (*vecAnimFrame)[State::STATE_ATTACK1])
		{
			bTimerAttack1 = false;
			m_fAnimationKeyFrameIndex_Attack1 = 0.f;

			m_IsSoundPlay[State::STATE_ATTACK1] = false;
			m_IsEffectPlay[State::STATE_ATTACK1] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
			CNetwork::GetInstance()->SendStopPacket();
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
			XMFLOAT4X4 matWorld = Matrix4x4::Identity();
			matWorld._41 = m_pObject->GetWorld()._41;
			matWorld._42 = m_pObject->GetWorld()._42;
			matWorld._43 = m_pObject->GetWorld()._43;
			CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK2], &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());

		}

		if (m_fAnimationKeyFrameIndex_Attack2 > (*vecAnimFrame)[State::STATE_ATTACK2])
		{
			bTimerAttack2 = false;
			m_fAnimationKeyFrameIndex_Attack2 = 0;

			m_IsSoundPlay[State::STATE_ATTACK2] = false;
			m_IsEffectPlay[State::STATE_ATTACK2] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
			CNetwork::GetInstance()->SendStopPacket();
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
			if (dynamic_cast<SelectPlayer*>(m_pObject)->GetIsWarrior())
				CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK3], &m_pObject->GetWorld(), m_pObject->GetNetRotAngle());
			else
				CEffect_Manager::GetInstance()->Play_SkillEffect(m_mapEffectName[State::STATE_ATTACK3], &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Attack3 > (*vecAnimFrame)[State::STATE_ATTACK3])
		{
			bTimerAttack3 = false;
			m_fAnimationKeyFrameIndex_Attack3 = 0;

			m_IsSoundPlay[State::STATE_ATTACK3] = false;
			m_IsEffectPlay[State::STATE_ATTACK3] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
			CNetwork::GetInstance()->SendStopPacket();
		}

	}

	if (bTimerDead == true)
	{
		//cout << m_fAnimationKeyFrameIndex_Dead << endl;
		if (m_bIsLife == true)
			m_fAnimationKeyFrameIndex_Dead += gt.DeltaTime() * 20;
		//m_iCurAnimFrame = m_fAnimationKeyFrameIndex_Attack3;

		if (m_fAnimationKeyFrameIndex_Dead + 1 > (*vecAnimFrame)[State::STATE_DEAD])
		{
			m_bIsLife = false;
			bTimerDead = false;
			//m_fAnimationKeyFrameIndex_Dead = 0;
		}

	}
	if (bTimerUltimate == true)
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

			auto pSelectPlayer = dynamic_cast<SelectPlayer*>(m_pObject);

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
			if (pSelectPlayer->GetIsWarrior())
			{
				pSelectPlayer->bIsUltimateState = true;
				pSelectPlayer->SetUltimateEffect(true);
			}
			CNetwork::GetInstance()->SendUltimateOnPacket(); // 진짜로 넘어간다
		}


	}

	if (bTimerRoll == true)
	{

		auto * m_pSelectPlayer = CManagement::GetInstance()->Find_Object(L"Layer_SelectPlayer");

		//m_pSelectPlayer->MoveForward(10.0f);
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
			CEffect_Manager::GetInstance()->Play_SkillEffect("hh", &m_pObject->GetWorld());
		}

		if (m_fAnimationKeyFrameIndex_Roll > (*vecAnimFrame)[State::STATE_ROLL])
		{
			bTimerRoll = false;
			m_fAnimationKeyFrameIndex_Roll = 0;

			m_IsSoundPlay[State::STATE_ROLL] = false;
			m_IsEffectPlay[State::STATE_ROLL] = false;

			m_pObject->GetAnimateMachine()->SetAnimState(STATE_IDLE);
			CNetwork::GetInstance()->SendStopPacket();
		}


	}

	if (bTimerHit)
	{

		auto * m_pSelectPlayer = CManagement::GetInstance()->Find_Object(L"Layer_SelectPlayer");

		//m_pSelectPlayer->MoveForward(10.0f);
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

			m_pObject->GetAnimateMachine()->SetAnimState(State::STATE_IDLE);
			CNetwork::GetInstance()->SendStopPacket();
		}


	}

}

void AnimateStateMachine_SelectPlayer::SetUltimateEffect(bool isUltimate)
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
		m_mapEffectName.emplace(State::STATE_ATTACK1, "Warrior_Turn");
		m_mapEffectName.emplace(State::STATE_ATTACK2, "UtimateAttack_2");
		m_mapEffectName.emplace(State::STATE_ATTACK3, "Ax_00");
		m_mapEffectName.emplace(State::STATE_ULTIMATE, "Trans_00");
	}
}

AnimateStateMachine_SelectPlayer * AnimateStateMachine_SelectPlayer::Create(CGameObject* pObj, wchar_t * pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END])
{
	AnimateStateMachine_SelectPlayer* pInstance = new AnimateStateMachine_SelectPlayer(pObj, pMachineName, SoundFrame, EffectFrame);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"AnimateStateMachine_SelectPlayer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void AnimateStateMachine_SelectPlayer::Free()
{
}
