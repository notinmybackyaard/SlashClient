#include "stdafx.h"
#include "MapObject.h"
#include "Define.h"
#include "ObjectManager.h"
#include "Component_Manager.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture_Manager.h"

CMapObject::CMapObject(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t* meshName)
	: CGameObject(d3dDevice, srv, srvSize)
{

	//size_t needed = ::wcstombs(NULL, &m_strMeshName[0], m_strMeshName.length());
	std::wstring output = meshName;
	size_t needed = ::wcstombs(NULL, &output[0], output.length());
	output.resize(needed);

	// real call
	::mbstowcs(&output[0], &m_strMeshName[0], m_strMeshName.length());

	m_strMeshName.assign(output.begin(), output.end());
	//m_strMeshName = meshName;
}

CMapObject::~CMapObject()
{
}

HRESULT CMapObject::Initialize()
{

	size_t needed = ::mbstowcs(NULL, &m_strMeshName[0], m_strMeshName.length());
	std::wstring output;
	output.resize(needed);

	// real call
	::mbstowcs(&output[0], &m_strMeshName[0], m_strMeshName.length());

	m_pMesh = dynamic_cast<StaticMesh*>(CComponent_Manager::GetInstance()->Clone_Component(const_cast<wchar_t*>(output.c_str())));
	if (nullptr == m_pMesh)
		return E_FAIL;

	m_strTexName = dynamic_cast<StaticMesh*>(m_pMesh)->GetTexName();
	output.clear();
	output.assign(m_strTexName.begin(), m_strTexName.end());
	auto Tex = CTexture_Manager::GetInstance()->Find_Texture(output, HEAP_DEFAULT);
	if (Tex == nullptr)
		return E_FAIL;

	Mat = new Material;
	Mat->Name = "BarrelMat";
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = Tex->Num;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;

	m_pTransCom = CTransform::Create(this);

	XMStoreFloat4x4(&World, XMMatrixScaling(0.1f, 0.1f, 0.1f));
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["Barrel"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["Barrel"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["Barrel"].BaseVertexLocation;
	Bounds = Geo->DrawArgs["Barrel"].Bounds;


	SetOOBB(XMFLOAT3(Bounds.Center.x * 0.1f, Bounds.Center.y * 0.1f, Bounds.Center.z * 0.1f), XMFLOAT3(Bounds.Extents.x * 0.1f, Bounds.Extents.y * 0.1f, Bounds.Extents.z * 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	return S_OK;
}

bool CMapObject::Update(const GameTimer & gt)
{
	CGameObject::Update(gt);
	m_pTransCom->Update_Component(gt);
	m_pMesh->Update(gt);
	//m_pCamera = CManagement::GetInstance()->Get_MainCam();
	//XMMATRIX view = m_pCamera->GetView();
	//XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	auto currObjectCB = m_pFrameResource->ObjectCB.get();


	////auto currObjectCB2 = m_pFrameResource->InstanceBuffer.get();

	XMMATRIX world = XMLoadFloat4x4(&m_pTransCom->GetWorld());
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);

	//XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	//// View space to the object's local space.
	//XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

	//// Transform the camera frustum from view space to the object's local space.
	//BoundingFrustum localSpaceFrustum;
	//mCamFrustum = *CManagement::GetInstance()->Get_CurScene()->Get_CamFrustum();
	//mCamFrustum.Transform(localSpaceFrustum, viewToLocal);

	//// Perform the box/frustum intersection test in local space.
	//if ((localSpaceFrustum.Contains(Bounds) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
	//{
	//	//cout << "보인당!" << endl;
	//	m_bIsVisiable = true;
	//	ObjectConstants objConstants;
	//	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	//	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
	//	objConstants.MaterialIndex = Mat->MatCBIndex;

	//	currObjectCB->CopyData(ObjCBIndex, objConstants);
	//}
	//else
	//{
	//	//cout << "안보인당!" << endl;
	//	m_bIsVisiable = false;
	//}
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
	objConstants.MaterialIndex = Mat->MatCBIndex;

	currObjectCB->CopyData(ObjCBIndex, objConstants);


	//////////////////////////////////////////////////


	// Next FrameResource need to be updated too.
	//NumFramesDirty--;
	auto currMaterialCB = m_pFrameResource->MaterialCB.get();

	XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

	if (m_IsClicked)
		Mat->DiffuseAlbedo = XMFLOAT4(1.f, 0.f, 0.f, 0.7f);
	else
		Mat->DiffuseAlbedo = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	MaterialConstants matConstants;
	matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;
	matConstants.FresnelR0 = Mat->FresnelR0;
	matConstants.Roughness = Mat->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);

	string a = "aaa";
	//if (m_IsAlpha)
	//	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);
	//else
	CObjectManager::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_NONALPHA_FORWARD, this);
	return true;
}

void CMapObject::Render(ID3D12GraphicsCommandList * cmdList)
{
	//if (m_bIsVisiable)
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

void CMapObject::SetMesh(wchar_t * meshName)
{
	StaticMesh* pMesh = dynamic_cast<StaticMesh*>(CComponent_Manager::GetInstance()->Clone_Component(meshName));
	if (pMesh == nullptr)
		return;
	m_pMesh = pMesh;

	wstring output;
	m_strTexName = dynamic_cast<StaticMesh*>(m_pMesh)->GetTexName();
	output.clear();
	output.assign(m_strTexName.begin(), m_strTexName.end());
	auto Tex = CTexture_Manager::GetInstance()->Find_Texture(output, HEAP_DEFAULT);
	if (Tex == nullptr)
		return;

	Mat->DiffuseSrvHeapIndex = Tex->Num;


	Geo = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["Barrel"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["Barrel"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["Barrel"].BaseVertexLocation;
	Bounds = Geo->DrawArgs["Barrel"].Bounds;
	wstring wstr(meshName);
	m_strMeshName.assign(wstr.begin(), wstr.end());
	SetOOBB(XMFLOAT3(Bounds.Center.x * 0.1f, Bounds.Center.y * 0.1f, Bounds.Center.z * 0.1f), XMFLOAT3(Bounds.Extents.x * 0.1f, Bounds.Extents.y * 0.1f, Bounds.Extents.z * 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

void CMapObject::SetTexture(Texture * tex)
{
	Mat->DiffuseSrvHeapIndex = tex->Num;
	m_strTexName = tex->Name;
}

void CMapObject::SetTexture(string texName)
{
	wstring wstr = L"";
	wstr.assign(texName.begin(), texName.end());
	
	SetTexture(CTexture_Manager::GetInstance()->Find_Texture(wstr, HEAP_DEFAULT));

}

void CMapObject::SetClicked(bool isCheck)
{
	if (m_IsClicked == isCheck)
		return;
	m_IsClicked = isCheck;
	/*if (isCheck)
		Mat->DiffuseAlbedo = XMFLOAT4(1.f, 0.f, 0.f, 0.7f);
	else
		Mat->DiffuseAlbedo = XMFLOAT4(1.f, 1.f, 1.f, 1.f);*/
}

CMapObject * CMapObject::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t* meshName)
{
	CMapObject* pInstance = new CMapObject(d3dDevice, srv, srvSize, meshName);
	
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CMapObject Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	CGameObject::Free();
}
