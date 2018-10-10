#include "stdafx.h"
#include "GeometryMesh.h"
#include "Terrain.h"
#include "Define.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Management.h"
#include "Renderer.h"

Terrain::Terrain(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
	: CGameObject(d3dDevice, srv, srvSize)
{
}

Terrain::~Terrain()
{
}

bool Terrain::Update(const GameTimer & gt)
{
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

	auto currObjectCB = m_pFrameResource->ObjectCB.get();


	/* CB Update */

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

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&GetWorld()));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));


	//// Perform the box/frustum intersection test in local space.
	//if ((localSpaceFrustum.Contains(Bounds) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
	//{
	//	//cout << "보인당!" << endl;
	//	m_bIsVisiable = true;

	//	

	//}
	//else
	//{
	//	//cout << "안보인당!" << endl;
	//	m_bIsVisiable = false;
	//}

	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);

	
	return true;

}

void Terrain::Render(ID3D12GraphicsCommandList * cmdList)
{
	if (/*m_bIsVisiable*/1)
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
	
}

HRESULT Terrain::Initialize()
{

	m_pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Geometry"));
	if (nullptr == m_pMesh)
		return E_FAIL;

	/*Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("stoneTex", CTexture_Manager::TEX_DEFAULT_2D);
	if (nullptr == tex)
		return E_FAIL;*/


	/* Material Build */
	Mat = new Material;
	Mat->Name = "TerrainMat";
	Mat->MatCBIndex =  m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = 0;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	/* CB(World,TextureTranform...) Build */

	XMStoreFloat4x4(&World, XMMatrixScaling(5.0f, 1.0f, 5.0f));// *XMMatrixRotationY(20.f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["grid"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["grid"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["grid"].BaseVertexLocation;

	return S_OK;

}

Terrain * Terrain::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
{
	Terrain* pInstance = new Terrain(d3dDevice, srv, srvSize);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Terrain Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Terrain::Free()
{
}





//#include "stdafx.h"
//#include "GeometryMesh.h"
//#include "Terrain.h"
//#include "Define.h"
//#include "Component_Manager.h"
//#include "Texture_Manager.h"
//#include "Management.h"
//#include "Renderer.h"
//
//Terrain::Terrain(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
//	: CGameObject(d3dDevice, srv, srvSize)
//{
//}
//
//Terrain::~Terrain()
//{
//}
//
//bool Terrain::Update(const GameTimer & gt)
//{
//	CGameObject::Update(gt);
//
//
//	m_pCamera = CManagement::GetInstance()->Get_MainCam();
//	XMMATRIX view = m_pCamera->GetView();
//	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
//
//
//	//XMFLOAT4X4  f4x4View;
//	//XMStoreFloat4x4(&f4x4View, m_pCamera->GetView());
//	//XMMATRIX matView = m_pCamera->GetView();
//
//	//XMStoreFloat4x4(&f4x4View, matView);
//	//f4x4View._41 = 0.f;
//	//f4x4View._42 = 0.f;
//	//f4x4View._43 = 0.f;
//
//	//matView = XMLoadFloat4x4(&f4x4View);
//
//	//XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(matView), matView);
//
//	//XMStoreFloat4x4(&f4x4View, matInvView);
//
//	////XMMATRIX matScale = XMMatrixScaling(m_pTransCom->GetScale().x, m_pTransCom->GetScale().y, m_pTransCom->GetScale().z);
//	//XMMATRIX matScale = XMMatrixScaling(1.f, 1.f, 1.f);
//	//XMFLOAT4X4 f4x4Scale;
//	//XMStoreFloat4x4(&f4x4Scale, matScale);
//
//	//XMMATRIX matRotX = XMMatrixRotationX(m_xmf3Rot.x);
//	//XMMATRIX matRotY = XMMatrixRotationY(m_xmf3Rot.y);
//	//XMMATRIX matRotZ = XMMatrixRotationZ(m_xmf3Rot.z);
//	//XMMATRIX matRot = XMMatrixIdentity();
//	//matRot = matRotX * matRotY * matRotZ;
//
//	//XMFLOAT4X4 f4x4Rot;
//	//XMStoreFloat4x4(&f4x4Rot, matRot);
//
//	//XMFLOAT4X4 f4x4World;
//	//XMStoreFloat4x4(&f4x4World, matScale * matRot);
//
//	//XMFLOAT3 pos;
//	//memcpy(&pos, &World._41, sizeof(float) * 3);
//
//	//World = Matrix4x4::Multiply(f4x4World, f4x4View);
//
//	//World._41 = pos.x;
//	//World._42 = pos.y;
//	//World._43 = pos.z;
//
//
//
//	//auto currObjectCB2 = m_pFrameResource->InstanceBuffer.get();
//
//	XMMATRIX world = XMLoadFloat4x4(&World);
//	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);
//
//	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
//
//	// View space to the object's local space.
//	XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);
//
//	// Transform the camera frustum from view space to the object's local space.
//	BoundingFrustum localSpaceFrustum;
//	mCamFrustum = *CManagement::GetInstance()->Get_CurScene()->Get_CamFrustum();
//	mCamFrustum.Transform(localSpaceFrustum, viewToLocal);
//
//	auto currObjectCB = m_pFrameResource->ObjectCB.get();
//
//
//	/* CB Update */
//
//	ObjectConstants objConstants;
//	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
//	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
//	objConstants.MaterialIndex = Mat->MatCBIndex;
//
//	currObjectCB->CopyData(ObjCBIndex, objConstants);
//
//	//////////////////////////////////////////////////
//
//	/* Material Update */
//	auto currMaterialCB = m_pFrameResource->MaterialCB.get();
//
//	XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);
//
//	MaterialConstants matConstants;
//	matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;
//	matConstants.FresnelR0 = Mat->FresnelR0;
//	matConstants.Roughness = Mat->Roughness;
//	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));
//
//	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;
//
//	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);
//
//	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&GetWorld()));
//	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
//
//
//	//// Perform the box/frustum intersection test in local space.
//	//if ((localSpaceFrustum.Contains(Bounds) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
//	//{
//	//	//cout << "보인당!" << endl;
//	//	m_bIsVisiable = true;
//
//	//	
//
//	//}
//	//else
//	//{
//	//	//cout << "안보인당!" << endl;
//	//	m_bIsVisiable = false;
//	//}
//
//	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);
//
//
//	return true;
//
//}
//
//void Terrain::Render(ID3D12GraphicsCommandList * cmdList)
//{
//	if (/*m_bIsVisiable*/1)
//	{
//		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
//		UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
//
//		auto objectCB = m_pFrameResource->ObjectCB->Resource();
//		auto matCB = m_pFrameResource->MaterialCB->Resource();
//
//		cmdList->IASetVertexBuffers(0, 1, &Geo->VertexBufferView());
//		cmdList->IASetIndexBuffer(&Geo->IndexBufferView());
//		cmdList->IASetPrimitiveTopology(PrimitiveType);
//
//		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
//		tex.Offset(Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);
//
//		Mat->DiffuseSrvHeapIndex;
//		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ObjCBIndex * objCBByteSize;
//		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + Mat->MatCBIndex*matCBByteSize;
//
//		cmdList->SetGraphicsRootConstantBufferView(4, objCBAddress);
//		//cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);
//		cmdList->SetGraphicsRootShaderResourceView(5, matCBAddress);
//
//		cmdList->SetGraphicsRootDescriptorTable(7, tex);
//
//		cmdList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
//	}
//
//}
//
//HRESULT Terrain::Initialize()
//{
//
//	m_pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Geometry"));
//	if (nullptr == m_pMesh)
//		return E_FAIL;
//
//	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("SelectBackground", CTexture_Manager::TEX_DEFAULT_2D);
//	if (nullptr == tex)
//	return E_FAIL;
//
//
//	/* Material Build */
//	Mat = new Material;
//	Mat->Name = "TerrainMat";
//	Mat->MatCBIndex = m_iMyObjectID;
//	Mat->DiffuseSrvHeapIndex = tex->Num;
//	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
//	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
//	Mat->Roughness = 0.3f;
//
//	/* CB(World,TextureTranform...) Build */
//
//	XMStoreFloat4x4(&World, XMMatrixScaling(10.0f, 1.0f, 10.0f));// *XMMatrixRotationY(20.f));
//	TexTransform = MathHelper::Identity4x4();
//	ObjCBIndex = m_iMyObjectID;
//
//	Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
//	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	IndexCount = Geo->DrawArgs["grid"].IndexCount;
//	StartIndexLocation = Geo->DrawArgs["grid"].StartIndexLocation;
//	BaseVertexLocation = Geo->DrawArgs["grid"].BaseVertexLocation;
//
//	return S_OK;
//
//}
//
//Terrain * Terrain::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
//{
//	Terrain* pInstance = new Terrain(d3dDevice, srv, srvSize);
//
//	if (FAILED(pInstance->Initialize()))
//	{
//		MSG_BOX(L"Terrain Created Failed");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void Terrain::Free()
//{
//}
