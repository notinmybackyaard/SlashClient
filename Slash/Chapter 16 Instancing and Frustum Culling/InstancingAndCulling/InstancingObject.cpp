#include "stdafx.h"
#include "InstancingObject.h"
#include "Define.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "Management.h"
#include "Transform.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Layer.h"
#include "TestScene.h"
//#include "Renderer.h"
#include "Network.h"

CInstancingObject* CInstancingObject::m_pAllInstObject[MAXINSTOBJECTID] = { nullptr };
unsigned long CInstancingObject::m_iAllInstObjectIndex = 0;
vector<std::unique_ptr<Material>>			CInstancingObject::mMaterials;


CInstancingObject::CInstancingObject(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* pMesh, int iSize)
	: CGameObject(d3dDevice, srv, srvSize)
{
	//while (m_pAllInstObject[m_iAllInstObjectIndex++])
	//{
	//	m_iAllInstObjectIndex %= MAXOBJECTID;
	//}
	//m_pAllInstObject[m_iAllInstObjectIndex] = this;
	//m_iMyObjectID = m_iAllInstObjectIndex;

	m_iMyInstObject = m_iAllInstObjectIndex;
	m_iAllInstObjectIndex += iSize;
	m_iAllObjectIndex += iSize;

	m_pwstrMeshName = pMesh;
	m_iSize = iSize;

}

CInstancingObject::~CInstancingObject()
{
}

void CInstancingObject::SaveSlidingVector(CGameObject * pobj, CGameObject * pCollobj)
{

	XMMATRIX world = XMLoadFloat4x4(&(pCollobj->GetTransform()->GetWorld()));

	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	// View space to the object's local space.

	// Transform the camera frustum from view space to the object's local space.

	BoundingOrientedBox mLocalPlayerBounds;
	// Transform the camera frustum from view space to the object's local space.
	pobj->m_xmOOBB.Transform(mLocalPlayerBounds, invWorld);

	BoundingBox WorldBounds = GetBounds();
	// Transform the camera frustum from view space to the object's local space.
	WorldBounds.Transform(WorldBounds, XMLoadFloat4x4(&(pCollobj->GetTransform()->GetWorld())));


	//XMFLOAT3 pCollObjPos2= pCollobj->m_pTransCom->m_f3Position;
	XMFLOAT3 Center = pCollobj->GetTransform()->GetPosition();
	XMFLOAT3 Player = pobj->GetPosition();
	XMFLOAT3 look = XMFLOAT3(pCollobj->GetTransform()->GetWorld()._21, pCollobj->GetTransform()->GetWorld()._22, pCollobj->GetTransform()->GetWorld()._23);
	XMFLOAT3 dirVector = Vector3::Subtract(Player, Center);   // 충돌 객체에서 플레이어로 가는 벡터

	float tanceta = dirVector.z / dirVector.x;
	float ceta = atan(tanceta) * 57.3248f;

	float extenttanceta = WorldBounds.Extents.z / WorldBounds.Extents.x;

	float extentceta = atan(extenttanceta) * 57.3248f;

	look = Vector3::Normalize(look);
	dirVector = Vector3::Normalize(dirVector);

	if (Player.x > Center.x && 0 < ceta && ceta< extentceta)
		pobj->m_MovingRefletVector = XMFLOAT3(1, 0, 0);
	if (Player.z > Center.z && extentceta <ceta && ceta < 90)
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, 1);
	if (Player.z > Center.z && -90 < ceta && ceta < -extentceta)
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, 1);
	if (Player.x < Center.x && -extentceta < ceta && ceta < 0)
		pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
	if (Player.x < Center.x && 0 < ceta && ceta < extentceta)
		pobj->m_MovingRefletVector = XMFLOAT3(-1, 0, 0);
	if (Player.z < Center.z && extentceta < ceta && ceta < 90)
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0,-1);
	if (Player.z < Center.z && -90 < ceta && ceta < -extentceta)
		pobj->m_MovingRefletVector = XMFLOAT3(0, 0, -1);
	if (Player.x > Center.x && -extentceta < ceta && ceta < 0) 
		pobj->m_MovingRefletVector = XMFLOAT3(1, 0, 0);
	
	XMFLOAT3 moveVec = Vector3::Normalize(Vector3::Subtract(pCollobj->GetPosition(), pobj->GetPosition()));

}

HRESULT CInstancingObject::Initialize()
{

	m_pMesh = dynamic_cast<StaticMesh*>(CComponent_Manager::GetInstance()->Clone_Component(m_pwstrMeshName));
	if (nullptr == m_pMesh)
		return E_FAIL;
	
	string TexName = m_pMesh->Get_TexName();

	auto tex = CTexture_Manager::GetInstance()->Find_Texture(TexName, CTexture_Manager::TEX_INST_2D);
	if (nullptr == tex)
		return E_FAIL;

	World = MathHelper::Identity4x4();
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = 0;

	Geo = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	InstanceCount = 0;
	IndexCount = Geo->DrawArgs["Barrel"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["Barrel"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["Barrel"].BaseVertexLocation;
	Bounds = Geo->DrawArgs["Barrel"].Bounds;

	Geo_Bounds = dynamic_cast<StaticMesh*>(m_pMesh)->m_Geometry[1].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	InstanceCount = 0;
	Element_Bounds.IndexCount = Geo_Bounds->DrawArgs["BarrelBounds"].IndexCount;
	Element_Bounds.StartIndexLocation = Geo_Bounds->DrawArgs["BarrelBounds"].StartIndexLocation;
	Element_Bounds.BaseVertexLocation = Geo_Bounds->DrawArgs["BarrelBounds"].BaseVertexLocation;

	vecInstances.resize(m_iSize);
	m_vecTransCom.resize(m_iSize);

	for (int i = 0; i < m_iSize; ++i)
	{
		vecInstances[i].World = XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.f, 0.f, 0.f, 1.0f);

		CTransform* pCom = CTransform::Create(this);
		pCom->GetPosition().x = i * 10.f;
		m_vecTransCom[i] = pCom;
		//XMStoreFloat4x4(&vecInstances[i].TexTransform, XMMatrixScaling(2.0f, 2.0f, 1.0f));
		vecInstances[i].TexTransform = TexTransform;
		//vecInstances[index].MaterialIndex = index % (mMaterials.size() - 1);
		//m_iMyInstObject;

		auto material = std::make_unique<Material>();
		material->Name = "material";
		material->MatCBIndex = m_iMyInstObject + i;
		material->DiffuseSrvHeapIndex = tex->Num;
		material->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.5f;

		mMaterials.push_back(std::move(material));

		vecInstances[i].MaterialIndex = m_iMyInstObject + i;

		/*cout << m_vecTransCom[i]->GetWorld()._11 << " " << m_vecTransCom[i]->GetWorld()._12 << " " << m_vecTransCom[i]->GetWorld()._13 << " " << m_vecTransCom[i]->GetWorld()._14 << endl;
		cout << m_vecTransCom[i]->GetWorld()._21 << " " << m_vecTransCom[i]->GetWorld()._22 << " " << m_vecTransCom[i]->GetWorld()._23 << " " << m_vecTransCom[i]->GetWorld()._24 << endl;
		cout << m_vecTransCom[i]->GetWorld()._31 << " " << m_vecTransCom[i]->GetWorld()._32 << " " << m_vecTransCom[i]->GetWorld()._33 << " " << m_vecTransCom[i]->GetWorld()._34 << endl;
		cout << m_vecTransCom[i]->GetWorld()._41 << " " << m_vecTransCom[i]->GetWorld()._42 << " " << m_vecTransCom[i]->GetWorld()._43 << " " << m_vecTransCom[i]->GetWorld()._44 << endl << endl;

		cout << vecInstances[i].World._11 << " " << vecInstances[i].World._12 << " " << vecInstances[i].World._13 << " " << vecInstances[i].World._14 << endl;
		cout << vecInstances[i].World._21 << " " << vecInstances[i].World._22 << " " << vecInstances[i].World._23 << " " << vecInstances[i].World._24 << endl;
		cout << vecInstances[i].World._31 << " " << vecInstances[i].World._32 << " " << vecInstances[i].World._33 << " " << vecInstances[i].World._34 << endl;
		cout << vecInstances[i].World._41 << " " << vecInstances[i].World._42 << " " << vecInstances[i].World._43 << " " << vecInstances[i].World._44 << endl << endl;*/
	}

	return S_OK;
}

bool CInstancingObject::Update(const GameTimer & gt)
{
	CTestScene* pScene = dynamic_cast<CTestScene*>(CManagement::GetInstance()->Get_CurScene());
	if (pScene == nullptr)
		return false;

	static bool firstTime = true;

	if (firstTime)
	{
		CNetwork::GetInstance()->SendMapObjectNumPacket(pScene->GetObjectCount());
		firstTime = false;
	}

	CGameObject::Update(gt);

	m_pCamera = CManagement::GetInstance()->Get_MainCam();
	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	auto currInstanceBuffer = m_pFrameResource->InstanceBuffer.get();
	auto& instanceData = vecInstances;

	int visibleInstanceCount = 0;

	int iTest = m_iMyInstObject;

	for (UINT i = 0; i < (UINT)m_iSize; ++i)
	{
		static int num = 0;

		Animate(gt, m_vecTransCom[i]);

		//XMMATRIX world = XMLoadFloat4x4(&instanceData[i].World);
		vecInstances[i].World = m_vecTransCom[i]->GetWorld();

		m_vecTransCom[i]->Update_Component(gt);
		
		if (pScene->GetObjectCount() >= ++num)
		{
			m_vecTransCom[i]->SendMapInitData();
		}

		instanceData[i].World = m_vecTransCom[i]->GetWorld();

		XMMATRIX world = XMLoadFloat4x4(&m_vecTransCom[i]->GetWorld());
		XMMATRIX texTransform = XMLoadFloat4x4(&instanceData[i].TexTransform);

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
			//cout << " 보인당" << endl;
			auto currInstanceBuffer = m_pFrameResource->InstanceBuffer.get();


			InstanceData data;
			XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
			data.MaterialIndex = instanceData[i].MaterialIndex;

			// Write the instance data to structured buffer for the visible objects. 보여? ㅎㅇ 
			currInstanceBuffer->CopyData(iTest++, data);
			visibleInstanceCount++;
		}
		else
		{
			//cout << " 안보인당 " << endl;
		}
	}

	//InstanceCount = visibleInstanceCount;

	InstanceCount = visibleInstanceCount;
	
	//cout << m_pwstrMeshName << " : " << endl;
	//cout << InstanceCount << endl;

	//cout << InstanceCount << endl;


	/* Material */
	auto currMaterialBuffer = m_pFrameResource->MaterialBuffer.get();


	for (int i = 0; i < m_iSize; ++i)
	{
		Material* mat = mMaterials[m_iMyInstObject + i].get();

		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}

	//for (auto& e : mMaterials)
	//{
	//	// Only update the cbuffer data if the constants have changed.  If the cbuffer
	//	// data changes, it needs to be updated for each FrameResource.
	//	Material* mat = e.get();
	//	if (mat->NumFramesDirty > 0)
	//	{
	//		XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

	//		MaterialData matData;
	//		matData.DiffuseAlbedo = mat->DiffuseAlbedo;
	//		matData.FresnelR0 = mat->FresnelR0;
	//		matData.Roughness = mat->Roughness;
	//		XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
	//		matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

	//		currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

	//		// Next FrameResource need to be updated too.
	//		mat->NumFramesDirty--;
	//	}
	//}
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(m_eMyRenderType, this);
	return true;
}

void CInstancingObject::Render(ID3D12GraphicsCommandList * cmdList)
{

	//RenderBounds(cmdList);
	
	cmdList->SetGraphicsRootDescriptorTable(3, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	//UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	cmdList->IASetVertexBuffers(0, 1, &Geo->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo->IndexBufferView());
	cmdList->IASetPrimitiveTopology(PrimitiveType);

	// Set the instance buffer to use for this render-item.  For structured buffers, we can bypass 
	// the heap and set as a root descriptor.

	auto instanceBuffer = m_pFrameResource->InstanceBuffer->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS Address = instanceBuffer->GetGPUVirtualAddress() + m_iMyInstObject * sizeof(InstanceData);
	cmdList->SetGraphicsRootShaderResourceView(0, Address);
	
	//InstanceCount = vecInstances.size();
	cmdList->DrawIndexedInstanced(IndexCount, InstanceCount, StartIndexLocation, BaseVertexLocation, 0);

	

}

void CInstancingObject::Animate(const GameTimer & gt)
{


}

void CInstancingObject::Animate(const GameTimer & gt, CTransform * transform)
{
	
	auto m_pPlayer = CManagement::GetInstance()->Find_Object(L"Layer_Player", 0); //이거 한번만 하게 바꿀껏


	m_pCamera = CManagement::GetInstance()->Get_MainCam();
	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	XMMATRIX world = XMLoadFloat4x4(&transform->GetWorld());

	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	// View space to the object's local space.

	// Transform the camera frustum from view space to the object's local space.

	BoundingOrientedBox mLocalPlayerBounds;
	// Transform the camera frustum from view space to the object's local space.
	m_pPlayer->m_xmOOBB.Transform(mLocalPlayerBounds, invWorld);

	BoundingBox WorldBounds = GetBounds();
	// Transform the camera frustum from view space to the object's local space.
	WorldBounds.Transform(WorldBounds, XMLoadFloat4x4(&(transform->GetWorld())));

	CTestScene* pScene = dynamic_cast<CTestScene*>(CManagement::GetInstance()->Get_CurScene());
	if (pScene == nullptr)
		return;
	pScene->GetObjectCount();
	
	if (mLocalPlayerBounds.Contains(GetBounds()) != DirectX::DISJOINT)
	{
		m_pPlayer->m_pCollider = this;
		m_pPlayer->m_pCollider->m_pTransCom = transform;
	}
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&(transform->GetWorld())));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}

CInstancingObject * CInstancingObject::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t* pMesh, int iSize)
{
	CInstancingObject* pInstance = new CInstancingObject(d3dDevice, srv, srvSize, pMesh, iSize);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CInstancingObject Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstancingObject::RenderBounds(ID3D12GraphicsCommandList * cmdList)
{
	//UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	cmdList->IASetVertexBuffers(0, 1, &Geo_Bounds->VertexBufferView());
	cmdList->IASetIndexBuffer(&Geo_Bounds->IndexBufferView());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// Set the instance buffer to use for this render-item.  For structured buffers, we can bypass 
	// the heap and set as a root descriptor.

	auto instanceBuffer = m_pFrameResource->InstanceBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->GetGPUVirtualAddress());



	
	cmdList->DrawIndexedInstanced(Element_Bounds.IndexCount, InstanceCount, Element_Bounds.StartIndexLocation, Element_Bounds.BaseVertexLocation, 0);





	////////////////////////////////////////////////////////////
}

void CInstancingObject::Free()
{
	
}
