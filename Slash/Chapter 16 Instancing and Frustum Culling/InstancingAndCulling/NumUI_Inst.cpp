#include "stdafx.h"
#include "NumUI_Inst.h"
#include "Define.h"
#include "GeometryMesh.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Management.h"
#include "InstancingObject.h"
#include "TestScene.h"
#include "Renderer.h"

NumUI_Inst::NumUI_Inst(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
	: UI(d3dDevice, srv, srvSize)
{
	m_iMyInstObject = CInstancingObject::m_iAllInstObjectIndex;
	CInstancingObject::m_iAllInstObjectIndex;
}

NumUI_Inst::~NumUI_Inst()
{
}

NumUI_Inst * NumUI_Inst::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
{
	NumUI_Inst*  pInstance = new NumUI_Inst(d3dDevice, srv, srvSize);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"NumUI Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT NumUI_Inst::Initialize()
{
	m_pMesh = dynamic_cast<GeometryMesh*>(CComponent_Manager::GetInstance()->Clone_Component(L"Com_Mesh_Geometry"));
	if (nullptr == m_pMesh)
		return E_FAIL;

	Texture* tex = CTexture_Manager::GetInstance()->Find_Texture("NumTex", CTexture_Manager::TEX_INST_2D);
	if (nullptr == tex)
		return E_FAIL;


	/* Material Build */
	Mat = new Material;
	Mat->Name = "TerrainMat";
	Mat->MatCBIndex = m_iMyInstObject;
	Mat->DiffuseSrvHeapIndex = tex->Num;
	Mat->DiffuseAlbedo = XMFLOAT4(1.f, 1.f, 0.f, 1.f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;
	Mat->MatTransform(3, 0) = 0.1f;
	Mat->MatTransform(3, 1) = 1.f;

	/* CB(World,TextureTranform...) Build */

	//XMStoreFloat4x4(&World, XMMatrixScaling(5.0f, 1.0f, 5.0f));// *XMMatrixRotationY(20.f));
	//World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&World, XMMatrixScaling(0.2f, 0.2f, 0.2f) *XMMatrixRotationX(-90.f));
	m_f4x4InitWorld = World;
	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<GeometryMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["grid"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["grid"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["grid"].BaseVertexLocation;

	return S_OK;
}

bool NumUI_Inst::Update(const GameTimer & gt)
{
	CGameObject::Update(gt);

	CTestScene* pScene = dynamic_cast<CTestScene*>(CManagement::GetInstance()->Get_CurScene());
	if (pScene == nullptr)
		return false;

	int objIdx = pScene->GetObjectCount();
	++objIdx;
	m_iMyInstObject = objIdx;

	m_pCamera = CManagement::GetInstance()->Get_MainCam();
	XMMATRIX view = m_pCamera->GetView();

	XMFLOAT4X4  f4x4View;
	XMStoreFloat4x4(&f4x4View, view);
	f4x4View._41 = 0.f;
	f4x4View._42 = 0.f;
	f4x4View._43 = 0.f;
	view = XMLoadFloat4x4(&f4x4View);

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	XMStoreFloat4x4(&f4x4View, invView);

	XMFLOAT4X4 matBillboard = Matrix4x4::Multiply(m_f4x4InitWorld, f4x4View);

	auto currInstanceBuffer = m_pFrameResource->InstanceBuffer.get();
	//auto& instanceData = vecInstances;

	int visibleInstanceCount = 0;

	int iTest = m_iMyInstObject;

	size_t iSize = m_vNum.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		static int num = 0;
		m_vNum[i].fStayTime += gt.DeltaTime() * 5.f;
		if (m_vNum[i].fStayTime > 1.5f)
			Erase_Vector_Element(m_vNum, i);

		//XMMATRIX world = XMLoadFloat4x4(&instanceData[i].World);

		//XMMATRIX world = XMLoadFloat4x4(&m_vecTransCom[i]->GetWorld());
		//XMMATRIX texTransform = XMLoadFloat4x4(&instanceData[i].TexTransform);
		matBillboard._41 = m_vNum[i].instData.World._41;
		matBillboard._42 = m_vNum[i].instData.World._42 + m_vNum[i].fStayTime;
		matBillboard._43 = m_vNum[i].instData.World._43;
		//XMMATRIX world = XMLoadFloat4x4(&m_vNum[i].instData.World);
		XMMATRIX world = XMLoadFloat4x4(&matBillboard);
		XMMATRIX texTransform = XMLoadFloat4x4(&m_vNum[i].instData.TexTransform);
		

		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

		// View space to the object's local space.
		XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

		// Transform the camera frustum from view space to the object's local space.
		BoundingFrustum localSpaceFrustum;
		mCamFrustum = *CManagement::GetInstance()->Get_CurScene()->Get_CamFrustum();
		mCamFrustum.Transform(localSpaceFrustum, viewToLocal);


		// Perform the box/frustum intersection test in local space.
	//	if ((localSpaceFrustum.Contains(Bounds) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
		{
			//cout << " º¸ÀÎ´ç" << endl;
			auto currInstanceBuffer = m_pFrameResource->InstanceBuffer.get();


			InstanceData data;
			XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
			//data.MaterialIndex = m_vNum[i].instData.MaterialIndex;
			data.MaterialIndex = m_iMyInstObject;
			//data.MaterialIndex = objIdx;

			// Write the instance data to structured buffer for the visible objects.
			//currInstanceBuffer->CopyData(objIdx++, data);
			currInstanceBuffer->CopyData(objIdx++, data);
			visibleInstanceCount++;
		}
	}

	//InstanceCount = visibleInstanceCount;

	//InstanceCount = visibleInstanceCount;

	//cout << m_pwstrMeshName << " : " << endl;
	//cout << InstanceCount << endl;

	//cout << InstanceCount << endl;


	/* Material */
	auto currMaterialBuffer = m_pFrameResource->MaterialBuffer.get();
	objIdx = m_iMyInstObject;

	for (int i = 0; i < iSize; ++i)
	{
		//Material* mat = mMaterials[m_iMyInstObject + i].get();

		if (Mat->NumFramesDirty > 0)
		{
			//Mat->MatTransform._31 = m_vNum[i].instData.TexTransform._41;
			XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = Mat->DiffuseAlbedo;
			matData.FresnelR0 = Mat->FresnelR0;
			matData.Roughness = Mat->Roughness;

			
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

			//currMaterialBuffer->CopyData(Mat->MatCBIndex, matData);
			
			//currMaterialBuffer->CopyData(m_iMyInstObject, matData);
			currMaterialBuffer->CopyData(m_iMyInstObject, matData);

			// Next FrameResource need to be updated too.
			Mat->NumFramesDirty--;
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
	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_UI_INST, this);
	return true;

}

void NumUI_Inst::Render(ID3D12GraphicsCommandList * cmdList)
{
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
	//cmdList->DrawIndexedInstanced(IndexCount, InstanceCount, StartIndexLocation, BaseVertexLocation, 0);
	cmdList->DrawIndexedInstanced(IndexCount, m_vNum.size(), StartIndexLocation, BaseVertexLocation, 0);

}

void NumUI_Inst::Add(int iNum, XMFLOAT3 f3Pos, bool isCritical)
{
	int iCnt = 0;
	for (int i = 4; i >= 0; --i)
	{
		if (iNum / int(pow(10, i)) > 0)
		{
			iCnt = i + 1;
			break;
		}
	}
	f3Pos.y += 5.f;
	float fNext = 1.f;
	float fX = f3Pos.x;
	float fZ = 0.001f;
	for (int i = 0; i < iCnt; ++i)
	{
		tagNumUI tNum;
		tNum.isCritical = isCritical;

		int iTexNum = (iNum / int(pow(10, iCnt - 1 - i)));

		iNum = iNum % int(pow(10, iCnt - 1 - i));

		tNum.instData.TexTransform._41 = float(iTexNum) / 10.f;
		tNum.instData.World = World;
		tNum.instData.World._41 = fX;
		fX += fNext;
		tNum.instData.World._42 = f3Pos.y;
		tNum.instData.World._43 = f3Pos.z + (fZ*i);

		m_vNum.push_back(tNum);
	}
	
}

void NumUI_Inst::Free()
{
}
