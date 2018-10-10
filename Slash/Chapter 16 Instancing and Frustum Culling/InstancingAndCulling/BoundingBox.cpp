#include "stdafx.h"
#include "BoundingBox.h"
#include "Define.h"
#include "GeometryMesh.h"
#include "Component_Manager.h"

CBoundingBox::CBoundingBox(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT3& f3Pos, XMFLOAT3& f3Scale)
	:CGameObject(d3dDevice, srv, srvSize)
	, m_f3Position(f3Pos)
	, m_f3Scale(f3Scale)
{

}

CBoundingBox::~CBoundingBox()
{
}

HRESULT CBoundingBox::Initialize()
{

	m_pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Geometry"));
	if (nullptr == m_pMesh)
		return E_FAIL;


	/* Material Build */
	Mat = new Material;
	Mat->Name = "BoxMat";
	Mat->MatCBIndex = 0;
	Mat->DiffuseSrvHeapIndex = 0;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	/* CB(World,TextureTranform...) Build */

	XMStoreFloat4x4(&World, XMMatrixScaling(10.0f, 10.0f, 10.0f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = 0;

	Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["sphere"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["sphere"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["sphere"].BaseVertexLocation;


	/////////////////////////

	return S_OK;
}

bool CBoundingBox::Update(const GameTimer & gt)
{
	CGameObject::Update(gt);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();

	/* CB Update */
	XMMATRIX world = XMLoadFloat4x4(&World);
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

void CBoundingBox::Render(ID3D12GraphicsCommandList * cmdList)
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

CBoundingBox * CBoundingBox::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, XMFLOAT3 & f3Pos, XMFLOAT3 & f3Scale)
{
	CBoundingBox* pInstance = new CBoundingBox(d3dDevice, srv, srvSize, f3Pos, f3Scale);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CBoundingBox Created Failed");
	}
	return pInstance;
}

void CBoundingBox::Free()
{
}

