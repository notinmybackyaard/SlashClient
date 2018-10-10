#include "stdafx.h"
#include "Effect.h"
#include "GeometryMesh.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Transform.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "StaticMesh.h"

CEffect::CEffect(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, EFFECT_INFO info)
	: CGameObject(d3dDevice, srv, srvSize)
	, m_tInfo(info)
	, m_ChangePos(info.S_Pos)
	, m_ChangeSize(info.S_Size)
	, m_ChangeRot(info.S_Rot)
	, m_ChangeColor(info.S_Color)
{

}

CEffect::~CEffect()
{
}

HRESULT CEffect::Initialize()
{
	wstring wstrMesh = wstring(m_tInfo.strMeshName.begin(), m_tInfo.strMeshName.end());
	string geoName;

	if (m_tInfo.strMeshName == "Com_Geometry")
	{
		m_pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Geometry"));
		if (nullptr == m_pMesh)
			return E_FAIL;
		m_tInfo.strTexName = "eye_moo_t_sun014";
		geoName = "grid";
	}
		
	else
	{
		m_pMesh = dynamic_cast<StaticMesh*>(CComponent_Manager::GetInstance()->Clone_Component(const_cast<wchar_t*>(wstrMesh.c_str())));
		if (nullptr == m_pMesh)
			return E_FAIL;
		m_tInfo.strTexName = dynamic_cast<StaticMesh*>(m_pMesh)->GetTexName();
		geoName = "Barrel";
	}

	//m_tFrame.f2maxFrame = XMFLOAT2(5.f, 6.f);
	//m_tFrame.f2FrameSize = XMFLOAT2(128.f, 171.f);
	//m_tFrame.fSpeed = 10.f;

	std::wstring output;
	output.clear();
	output.assign(m_tInfo.strTexName.begin(), m_tInfo.strTexName.end());
	auto Tex = CTexture_Manager::GetInstance()->Find_Texture(output, HEAP_TEXTURE_EFFECT);
	if (Tex == nullptr)
		return E_FAIL;

	m_pTransCom = CTransform::Create(this);

	/* Material Build */
	Mat = new Material;
	Mat->Name = "TerrainMat";
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = Tex->Num;
	Mat->DiffuseAlbedo = m_tInfo.S_Color;// XMFLOAT4(1.0f, 1.0f, 1.0f, 1.f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	/* CB(World,TextureTranform...) Build */

	m_pTransCom->Scaling(m_tInfo.S_Size);
	m_pTransCom->Translate(m_tInfo.S_Pos);
	m_pTransCom->Rotation(m_tInfo.S_Rot);

	XMStoreFloat4x4(&World, XMMatrixScaling(3.0f, 1.0f, 3.0f) * XMMatrixTranslation(0.f, 3.f, 0.f));// *XMMatrixRotationX(-90.f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	if (m_tInfo.strMeshName == "Com_Geometry")
		Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
	else
		Geo = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs[geoName].IndexCount;
	StartIndexLocation = Geo->DrawArgs[geoName].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs[geoName].BaseVertexLocation;

	Mat->MatTransform(3, 0) = 1;
	Mat->MatTransform(3, 1) = 1;
	return S_OK;
}

bool CEffect::Update(const GameTimer & gt)
{
	if (m_IsEnable == false)
		return true;

	if (!m_IsPlay)
		Update_Default(gt);
	else
		Update_Play(gt);

	return true;
}

void CEffect::Render(ID3D12GraphicsCommandList * cmdList)
{
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

	cmdList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);

}

void CEffect::Update_Default(const GameTimer & gt)
{
	CGameObject::Update(gt);

	if (m_pCamera == nullptr)
		m_pCamera = CObjectManager::GetInstance()->Get_MainCam();

	m_pTransCom->GetPosition() = m_tInfo.S_Pos;
	m_pTransCom->GetScale() = m_tInfo.S_Size;
	m_pTransCom->GetRotation() = m_tInfo.S_Rot;

	m_pTransCom->Update_Component(gt);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();

	if (m_tInfo.isBillboard)
		Update_Billboard(gt);

	//XMMATRIX world = XMLoadFloat4x4(&f4x4View);
	XMMATRIX world = XMLoadFloat4x4(&m_pTransCom->GetWorld());
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
	objConstants.MaterialIndex = Mat->MatCBIndex;

	currObjectCB->CopyData(ObjCBIndex, objConstants);

	//////////////////////////////////////////////////

	/* Material Update */
	//Mat->NumFramesDirty = gNumFrameResources;
	auto currMaterialCB = m_pFrameResource->MaterialCB.get();

	XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

	MaterialConstants matConstants;
	matConstants.DiffuseAlbedo = m_tInfo.S_Color;// Mat->DiffuseAlbedo;
	matConstants.FresnelR0 = Mat->FresnelR0;
	matConstants.Roughness = Mat->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));
	

	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);
	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, this);
}

void CEffect::Update_Play(const GameTimer & gt)
{
	float deltaTime = gt.DeltaTime();
	m_fTimeDeltaAcc += deltaTime;

	if (m_fTimeDeltaAcc < m_tInfo.StartTime)
		return;

	m_fLifeTimeAcc += deltaTime;

	if(m_IsFrame)
		MoveFrame(gt);
	
	//if(!m_IsFrame || !m_tFrame.isEndbyCnt)
	if (m_fLifeTimeAcc > m_tInfo.LifeTime)				// > 이펙트 라이프타임이 끝나면 지움 현재는 다시 처음으로 돌림
		SetPlay(true);

	CGameObject::Update(gt);

	if (m_pCamera == nullptr)
		m_pCamera = CObjectManager::GetInstance()->Get_MainCam();

	float fDivTime = m_tInfo.LifeTime / deltaTime;

	m_ChangePos = Vector3::Divide(Vector3::Subtract(m_tInfo.E_Pos, m_tInfo.S_Pos), fDivTime);
	m_ChangeSize = Vector3::Divide(Vector3::Subtract(m_tInfo.E_Size, m_tInfo.S_Size), fDivTime);
	m_ChangeRot = Vector3::Divide(Vector3::Subtract(m_tInfo.E_Rot, m_tInfo.S_Rot), fDivTime);
	m_ChangeColor = Vector4::Divide(Vector4::Subtract(m_tInfo.E_Color, m_tInfo.S_Color), fDivTime);

	m_pTransCom->GetPosition() += m_ChangePos;
	m_pTransCom->GetScale() += m_ChangeSize;
	m_pTransCom->GetRotation() += m_ChangeRot;

	m_pTransCom->Update_Component(gt);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();

	if (m_tInfo.isBillboard)
		Update_Billboard(gt);

	/*XMFLOAT4X4  f4x4View;
	XMStoreFloat4x4(&f4x4View, m_pCamera->GetView());
	XMMATRIX matView = m_pCamera->GetView();


	XMStoreFloat4x4(&f4x4View, matView);
	f4x4View._41 = 0.f;
	f4x4View._42 = 0.f;
	f4x4View._43 = 0.f;

	matView = XMLoadFloat4x4(&f4x4View);

	XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(matView), matView);

	XMStoreFloat4x4(&f4x4View, matInvView);

	XMMATRIX matScale = XMMatrixScaling(m_pTransCom->GetScale().x, m_pTransCom->GetScale().y, m_pTransCom->GetScale().z);
	XMFLOAT4X4 f4x4Scale;
	XMStoreFloat4x4(&f4x4Scale, matScale);

	XMMATRIX matRotX = XMMatrixRotationX(XMConvertToRadians(m_pTransCom->GetRotation().x));
	XMMATRIX matRotY = XMMatrixRotationY(XMConvertToRadians(m_pTransCom->GetRotation().y));
	XMMATRIX matRotZ = XMMatrixRotationZ(XMConvertToRadians(m_pTransCom->GetRotation().z));
	XMMATRIX matRot = XMMatrixIdentity();
	matRot = matRotX * matRotY * matRotZ;

	XMFLOAT4X4 f4x4Rot;
	XMStoreFloat4x4(&f4x4Rot, matRot);

	XMFLOAT4X4 f4x4World;
	XMStoreFloat4x4(&f4x4World, matScale * matRot);

	m_pTransCom->GetWorld() = Matrix4x4::Multiply(f4x4World, f4x4View);

	m_pTransCom->GetWorld()._41 = m_pTransCom->GetPosition().x;
	m_pTransCom->GetWorld()._42 = m_pTransCom->GetPosition().y;
	m_pTransCom->GetWorld()._43 = m_pTransCom->GetPosition().z;*/

	//XMMATRIX world = XMLoadFloat4x4(&f4x4View);
	XMMATRIX world = XMLoadFloat4x4(&m_pTransCom->GetWorld());
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
	objConstants.MaterialIndex = Mat->MatCBIndex;

	currObjectCB->CopyData(ObjCBIndex, objConstants);

	//////////////////////////////////////////////////

	/* Material Update */
	auto currMaterialCB = m_pFrameResource->MaterialCB.get();

	XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

	MaterialConstants matConstants;
	Mat->DiffuseAlbedo += m_ChangeColor;
	matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;// Mat->DiffuseAlbedo;
	matConstants.FresnelR0 = Mat->FresnelR0;
	matConstants.Roughness = Mat->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);
	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA_DEFAULT, this);
}

void CEffect::Update_Billboard(const GameTimer & gt)
{
	XMFLOAT4X4  f4x4View;
	XMStoreFloat4x4(&f4x4View, m_pCamera->GetView());
	XMMATRIX matView = m_pCamera->GetView();

	XMStoreFloat4x4(&f4x4View, matView);
	f4x4View._41 = 0.f;
	f4x4View._42 = 0.f;
	f4x4View._43 = 0.f;

	matView = XMLoadFloat4x4(&f4x4View);

	XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(matView), matView);

	XMStoreFloat4x4(&f4x4View, matInvView);

	XMMATRIX matScale = XMMatrixScaling(m_pTransCom->GetScale().x, m_pTransCom->GetScale().y, m_pTransCom->GetScale().z);
	XMFLOAT4X4 f4x4Scale;
	XMStoreFloat4x4(&f4x4Scale, matScale);

	XMMATRIX matRotX = XMMatrixRotationX(m_pTransCom->GetRotation().x);
	XMMATRIX matRotY = XMMatrixRotationY(m_pTransCom->GetRotation().y);
	XMMATRIX matRotZ = XMMatrixRotationZ(m_pTransCom->GetRotation().z);
	XMMATRIX matRot = XMMatrixIdentity();
	matRot = matRotX * matRotY * matRotZ;

	XMFLOAT4X4 f4x4Rot;
	XMStoreFloat4x4(&f4x4Rot, matRot);

	XMFLOAT4X4 f4x4World;
	XMStoreFloat4x4(&f4x4World, matScale * matRot);

	m_pTransCom->GetWorld() = Matrix4x4::Multiply(f4x4World, f4x4View);

	m_pTransCom->GetWorld()._41 = m_pTransCom->GetPosition().x;
	m_pTransCom->GetWorld()._42 = m_pTransCom->GetPosition().y;
	m_pTransCom->GetWorld()._43 = m_pTransCom->GetPosition().z;
}

void CEffect::SetMesh(wchar_t* meshName)
{
	string geoName;
	if (!wcscmp(meshName, L"Com_Geometry"))
	{
		GeometryMesh* pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Geometry"));
		if (pMesh == nullptr)
			return;
		m_pMesh = pMesh;

		Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
		geoName = "grid";
	}
	else
	{
		StaticMesh* pMesh = dynamic_cast<StaticMesh*>(CComponent_Manager::GetInstance()->Clone_Component(meshName));
		if (pMesh == nullptr)
			return;
		m_pMesh = pMesh;

		wstring output;
		m_tInfo.strTexName = dynamic_cast<StaticMesh*>(m_pMesh)->GetTexName();
		output.clear();
		output.assign(m_tInfo.strTexName.begin(), m_tInfo.strTexName.end());
		auto Tex = CTexture_Manager::GetInstance()->Find_Texture(output, HEAP_DEFAULT);
		if (Tex == nullptr)
			return;

		Mat->DiffuseSrvHeapIndex = Tex->Num;

		Geo = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[0].get();
		geoName = "Barrel";
	}

	//Geo = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs[geoName].IndexCount;
	StartIndexLocation = Geo->DrawArgs[geoName].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs[geoName].BaseVertexLocation;
	Bounds = Geo->DrawArgs[geoName].Bounds;
	wstring wstr(meshName);
	m_strMeshName.assign(wstr.begin(), wstr.end());
	m_tInfo.strMeshName.assign(wstr.begin(), wstr.end());
	//SetOOBB(XMFLOAT3(Bounds.Center.x * 0.1f, Bounds.Center.y * 0.1f, Bounds.Center.z * 0.1f), XMFLOAT3(Bounds.Extents.x * 0.1f, Bounds.Extents.y * 0.1f, Bounds.Extents.z * 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

void CEffect::SetTexture(Texture * tex)
{
	Mat->DiffuseSrvHeapIndex = tex->Num;
	m_tInfo.strTexName = tex->Name;
}

void CEffect::SetTexture(string texName)
{
	wstring wstr = L"";
	wstr.assign(texName.begin(), texName.end());

	SetTexture(CTexture_Manager::GetInstance()->Find_Texture(wstr, HEAP_TEXTURE_EFFECT));
}

void CEffect::SetPlay(bool _isPlay)
{
	m_IsPlay = _isPlay;

	m_fTimeDeltaAcc = 0.f;
	m_fLifeTimeAcc = 0.f;

	m_pTransCom->GetPosition() = m_tInfo.S_Pos;
	m_pTransCom->GetScale() = m_tInfo.S_Size;
	m_pTransCom->GetRotation() = m_tInfo.S_Rot;
	Mat->DiffuseAlbedo = m_tInfo.S_Color;
}

void CEffect::SetIsFrame(bool _isFrame)
{
	m_IsFrame = _isFrame;

	m_tFrame.f2curFrame = XMFLOAT2(0.f, 0.f);
	m_tFrame.fFrameAcc = 0.f;
	
}

void CEffect::MoveFrame(const GameTimer& gt)
{
	m_tFrame.fFrameAcc += gt.DeltaTime() * m_tFrame.fSpeed;
	//if (m_tFrame.fFrameAcc > m_tFrame.f2FrameSize.x)
	if(m_tFrame.fFrameAcc > 1.f)
	{
		m_tFrame.f2curFrame.x += 1.f;
		m_tFrame.fFrameAcc = 0.f;
		if (m_tFrame.f2curFrame.x >= m_tFrame.f2maxFrame.x)
		{
			m_tFrame.f2curFrame.x = 0.f;
			m_tFrame.f2curFrame.y += 1.f;
			if (m_tFrame.f2curFrame.y >= m_tFrame.f2maxFrame.y)
			{
				m_tFrame.f2curFrame.x = 0.f;
				m_tFrame.f2curFrame.y = 0.f;
				
				if (m_tFrame.isEndbyCnt)
				{
					++m_tFrame.iCurCnt;
					if (m_tFrame.iCurCnt > m_tFrame.iPlayCnt)
						m_IsEnable = false;			// > 끝내기
				}
			}
		}
	}
	TexTransform._41 = m_tFrame.f2curFrame.x / m_tFrame.f2maxFrame.x;
	TexTransform._42 = m_tFrame.f2curFrame.y / m_tFrame.f2maxFrame.y;

	Mat->MatTransform(3, 0) = 1 / m_tFrame.f2maxFrame.x;
	Mat->MatTransform(3, 1) = 1 / m_tFrame.f2maxFrame.y;
}

CEffect * CEffect::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, EFFECT_INFO info)
{
	CEffect* pInstance = new CEffect(d3dDevice, srv, srvSize, info);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CEffect Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect::Free()
{
	CGameObject::Free();
}
