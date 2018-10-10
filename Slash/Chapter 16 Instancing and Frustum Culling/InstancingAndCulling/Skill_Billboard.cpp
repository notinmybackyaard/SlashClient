#include "stdafx.h"
#include "Skill_Billboard.h"
#include "GeometryMesh.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Management.h"
#include "Camera.h"
#include "Transform.h"

CSkill_Billboard::CSkill_Billboard(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string TexName)
	: CGameObject(d3dDevice, srv, srvSize)
	, m_strTexName(TexName)
{
}

CSkill_Billboard::~CSkill_Billboard()
{
}

HRESULT CSkill_Billboard::Initialize()
{
	m_pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Geometry"));
	if (nullptr == m_pMesh)
		return E_FAIL;

	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture(m_strTexName, CTexture_Manager::TEX_DEFAULT_2D);
	if (nullptr == tex)
	return E_FAIL;

	m_pTransCom = CTransform::Create(this);

	/* Material Build */
	Mat = new Material;
	Mat->Name = "TerrainMat";
	Mat->MatCBIndex = 2;
	Mat->DiffuseSrvHeapIndex = tex->Num;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	/* CB(World,TextureTranform...) Build */

	m_pTransCom->Scaling(3.f, 1.f, 3.f);
	m_pTransCom->Translate(0.f, 3.f, 0.f);
	m_pTransCom->Rotation(-90.f, 0.f, 0.f);

	XMStoreFloat4x4(&World, XMMatrixScaling(3.0f, 1.0f, 3.0f) * XMMatrixTranslation(0.f, 3.f, 0.f));// *XMMatrixRotationX(-90.f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["grid"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["grid"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["grid"].BaseVertexLocation;


	return S_OK;

}

bool CSkill_Billboard::Update(const GameTimer & gt)
{
	CGameObject::Update(gt);

	if (m_pCamera == nullptr)
		m_pCamera = CManagement::GetInstance()->Get_MainCam();
	m_pTransCom->Update_Component(gt);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();

	XMFLOAT4X4  f4x4View;
	XMStoreFloat4x4(&f4x4View, m_pCamera->GetView());
	XMMATRIX matView = m_pCamera->GetView();

	//XMMatrixInverse(&XMMatrixDeterminant(matView), matView);

	XMStoreFloat4x4(&f4x4View, matView);
	f4x4View._41 = 0.f;
	f4x4View._42 = 0.f;
	f4x4View._43 = 0.f;

	//m_pTransCom->GetWorld() = 

	XMMATRIX matScale = XMMatrixScaling(m_pTransCom->GetScale().x, m_pTransCom->GetScale().y, m_pTransCom->GetScale().z);
	XMFLOAT4X4 f4x4Scale;
	XMStoreFloat4x4(&f4x4Scale, matScale);

	m_pTransCom->GetWorld() = Matrix4x4::Multiply(f4x4Scale, f4x4View);

	m_pTransCom->GetWorld()._41 = m_pTransCom->GetPosition().x;
	m_pTransCom->GetWorld()._42 = m_pTransCom->GetPosition().y;
	m_pTransCom->GetWorld()._43 = m_pTransCom->GetPosition().z;

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
	matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;
	matConstants.FresnelR0 = Mat->FresnelR0;
	matConstants.Roughness = Mat->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);
	return true;
}

void CSkill_Billboard::Render(ID3D12GraphicsCommandList * cmdList)
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

CSkill_Billboard * CSkill_Billboard::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string TexName)
{
	CSkill_Billboard* pInstance = new CSkill_Billboard(d3dDevice, srv, srvSize, TexName);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CSkill_Billboard Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Billboard::Free()
{
}
