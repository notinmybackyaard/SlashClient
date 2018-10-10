#include "stdafx.h"
#include "Skill.h"
#include "Define.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Management.h"
#include "BillboardMesh.h"
#include "Transform.h"

CSkill::CSkill(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
	: CGameObject(d3dDevice, srv, srvSize)
{

}

CSkill::~CSkill()
{
}

HRESULT CSkill::Initialize()
{
	m_pMesh = dynamic_cast<CBillboardMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Billboard"));
	if (m_pMesh == nullptr)
		return E_FAIL;

	auto Tex = CTexture_Manager::GetInstance()->Find_Texture("treeArrayTex", CTexture_Manager::TEX_DEFAULT_BILLBOARD);
	if (Tex == nullptr)
		return E_FAIL;

	m_pTransCom = CTransform::Create(this);

	Mat = new Material;
	Mat->Name = "treeSprites";
	Mat->MatCBIndex = 0;
	Mat->DiffuseSrvHeapIndex = Tex->Num;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	Mat->Roughness = 0.125f;

	/* CB(World,TextureTranform...) Build */

	XMStoreFloat4x4(&World, XMMatrixScaling(10.0f, 10.0f, 10.0f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<CBillboardMesh*>(m_pMesh)->Get_Geometry();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	IndexCount = Geo->DrawArgs["points"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["points"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["points"].BaseVertexLocation;

	return S_OK;
}

bool CSkill::Update(const GameTimer & gt)
{
	CGameObject::Update(gt);
	m_pTransCom->Update_Component(gt);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();

	/* CB Update */
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

void CSkill::Render(ID3D12GraphicsCommandList * cmdList)
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

	cmdList->SetGraphicsRootDescriptorTable(8, tex);

	cmdList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}

CSkill * CSkill::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
{
	CSkill* pInstance = new CSkill(d3dDevice, srv, srvSize);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CSkill Created Failed");
	}
	return pInstance;
}

void CSkill::Free()
{

}

