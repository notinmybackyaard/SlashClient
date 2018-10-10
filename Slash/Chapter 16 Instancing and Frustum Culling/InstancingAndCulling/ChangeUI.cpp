
#include "stdafx.h"
#include "ChangeUI.h"

#include "UIMesh.h"
#include "Management.h"
#include "Renderer.h"
#include "Component_Manager.h"
#include "Define.h"

ChangeUI::ChangeUI(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT2 _move, XMFLOAT2 _scale, float _size, int diffuseSrvHeapIndex, bool isCon, float fZ, float fStartTime)
	:UI(d3dDevice, srv, srvSize)
{
	move = _move;
	scale = _scale;
	size = _size;
	m_fZ = fZ;
	m_iDiffuseSrvHeapIndex = diffuseSrvHeapIndex;
	m_fStartTime = fStartTime;
	m_IsContinue = isCon;
	m_xmf3Scale.x = 1.f;
	m_xmf3Scale.y = 1.f;
	m_xmf3Scale.z = 1.f;
}

ChangeUI::ChangeUI(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t * pMeshName, int diffuseSrvHeapIndex, bool isCon)
	:UI(d3dDevice, srv, srvSize)
{
	m_iDiffuseSrvHeapIndex = diffuseSrvHeapIndex;
	m_IsContinue = isCon;
	m_xmf3Scale.x = 1.f;
	m_xmf3Scale.y = 1.f;
	m_xmf3Scale.z = 1.f;
}


ChangeUI::~ChangeUI()
{
}

ChangeUI * ChangeUI::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, XMFLOAT2 move, XMFLOAT2 scale, float size, int diffuseSrvHeapIndex, bool isCon, float fZ, float fStartTime)
{
	ChangeUI* pInstance = new ChangeUI(d3dDevice, srv, srvSize, move, scale, size, diffuseSrvHeapIndex, isCon, fZ, fStartTime);

	if (FAILED(pInstance->Initialize(move, scale, size)))
	{
		MSG_BOX(L"ChangeUI Created Failed");
		Safe_Release(pInstance);
	}


	return pInstance;
}

ChangeUI * ChangeUI::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t * pMeshName, int diffuseSrvHeapIndex, bool isCon)
{
	ChangeUI* pInstance = new ChangeUI(d3dDevice, srv, srvSize,pMeshName, diffuseSrvHeapIndex, isCon);

	if (FAILED(pInstance->Initialize(pMeshName, diffuseSrvHeapIndex)))
	{
		MSG_BOX(L"ChangeUI Created Failed");
		Safe_Release(pInstance);
	}


	return pInstance;
}

bool ChangeUI::Update(const GameTimer & gt)
{
	if (!m_isPlay)
	{
		return true;
	}
		

	m_fAccTime += gt.DeltaTime();
	if (m_fStartTime > m_fAccTime)
		return true;

	CGameObject::Update(gt);


	//XMMATRIX world = XMLoadFloat4x4(&f4x4View);

	auto currObjectCB = m_pFrameResource->ObjectCB.get();

	XMMATRIX world = XMLoadFloat4x4(&World);
	XMMATRIX texTransform = XMLoadFloat4x4(&TexTransform);

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
	objConstants.MaterialIndex = Mat->MatCBIndex;

	currObjectCB->CopyData(ObjCBIndex, objConstants);


	auto currMaterialCB = m_pFrameResource->MaterialCB.get();

	XMMATRIX matTransform = XMLoadFloat4x4(&Mat->MatTransform);

	MaterialConstants matConstants;

	if (m_IsChange && !m_isEnd)
	{

		float fDivTime = m_fChangeTime / gt.DeltaTime();

		m_ChangeColor = Vector4::Divide(Vector4::Subtract(m_xmf4ColorChange, m_xmInitColor), fDivTime);

		if (m_ChangeValue)
		{
			Mat->DiffuseAlbedo = Vector4::Add(Mat->DiffuseAlbedo, m_ChangeColor);
		}

		else
		{
			Mat->DiffuseAlbedo = Vector4::Subtract(Mat->DiffuseAlbedo, m_ChangeColor);
		}
		m_fTimeAccc += gt.DeltaTime();

		if (m_fTimeAccc > m_fChangeTime)
		{
			if (!m_IsContinue)
			{
				if (!m_isRemain)
				{
					m_isPlay = false;
					Mat->DiffuseAlbedo = m_xmInitColor;// XMFLOAT4(1, 1, 1, 1);
					m_fAccTime = 0.f;
					m_fTimeAccc = 0.f;
					m_isEnd = true;
					return true;
				}
				else
				{
					m_isEnd = true;
					m_fAccTime = 0.f;
					m_fTimeAccc = 0.f;
					Mat->DiffuseAlbedo = m_xmf4ColorChange;
				}
			}
			else
			{
				m_ChangeValue = !m_ChangeValue;
			}

			m_fTimeAccc = 0.f;
		}
	}

	if (m_isFrame)
		MoveFrame(gt);

	matConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;// Mat->DiffuseAlbedo;
	matConstants.FresnelR0 = Mat->FresnelR0;
	matConstants.Roughness = Mat->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	matConstants.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

	currMaterialCB->CopyData(Mat->MatCBIndex, matConstants);


	CManagement::GetInstance()->GetRenderer()->Add_RenderGroup(CRenderer::RENDER_UICHANGE, this);

	return true;
}

void ChangeUI::SetUI(float size, float moveX, float moveY, float scaleX, float scaleY)
{
}

void ChangeUI::Render(ID3D12GraphicsCommandList * cmdList)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_pFrameResource->ObjectCB->Resource();
	auto matCB = m_pFrameResource->MaterialCB->Resource();

	cmdList->IASetVertexBuffers(0, 1, &Geo->VertexBufferView());
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


	cmdList->DrawInstanced(6, 1, 0, 0);
}

HRESULT ChangeUI::Initialize(XMFLOAT2 move, XMFLOAT2 scale, float size)
{
	m_pMesh = UIMesh::Create(m_d3dDevice, move, scale, size, m_fZ);

	/* Material Build */
	Mat = new Material;
	Mat->Name = "TerrainMat";
	//Mat->MatCBIndex = 2;
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = m_iDiffuseSrvHeapIndex;//7;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_xmInitColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;
	Mat->MatTransform = MathHelper::Identity4x4();

	/* CB(World,TextureTranform...) Build */

	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<UIMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["UI"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["UI"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["UI"].BaseVertexLocation;

	return S_OK;
}

HRESULT ChangeUI::Initialize(wchar_t * pMeshName, int diffuseSrvHeapIndex)
{
	m_pMesh = dynamic_cast<UIMesh*>(CComponent_Manager::GetInstance()->Clone_Component(pMeshName));
	if (m_pMesh == nullptr)
		return E_FAIL;
	/* Material Build */
	Mat = new Material;
	Mat->Name = "TerrainMat";
	//Mat->MatCBIndex = 2;
	Mat->MatCBIndex = m_iMyObjectID;
	Mat->DiffuseSrvHeapIndex = m_iDiffuseSrvHeapIndex;//7;
	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_xmInitColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Mat->Roughness = 0.3f;
	Mat->MatTransform = MathHelper::Identity4x4();

	/* CB(World,TextureTranform...) Build */

	TexTransform = MathHelper::Identity4x4();
	ObjCBIndex = m_iMyObjectID;

	Geo = dynamic_cast<UIMesh*>(m_pMesh)->m_Geometry[0].get();
	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	IndexCount = Geo->DrawArgs["UI"].IndexCount;
	StartIndexLocation = Geo->DrawArgs["UI"].StartIndexLocation;
	BaseVertexLocation = Geo->DrawArgs["UI"].BaseVertexLocation;

	return S_OK;
}

void ChangeUI::SetChangeInfo(XMFLOAT4 _xm, float _time)
{
	m_fChangeTime = _time;
	m_xmf4ColorChange = _xm;
}

void ChangeUI::SetisChange(bool _isChange)
{
	m_IsChange = _isChange;

	Mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_fTimeAccc = 0.f;
}

void ChangeUI::SetFrameInfo(UV_FRAME_INFO info)
{
	m_isFrame = true;
	m_tFrame = info;
}

void ChangeUI::MoveFrame(const GameTimer& gt)
{
	m_tFrame.fFrameAcc += gt.DeltaTime() * m_tFrame.fSpeed;
	//if (m_tFrame.fFrameAcc > m_tFrame.f2FrameSize.x)
	if (m_tFrame.fFrameAcc > 1.f)
	{
		m_tFrame.f2curFrame.x += 1.f;
		m_tFrame.fFrameAcc = 0.f;
		if (m_tFrame.f2curFrame.x >= m_tFrame.f2maxFrame.x)
		{
			m_tFrame.f2curFrame.x = 0.f;
			m_tFrame.f2curFrame.y += 1.f;
			if (m_tFrame.f2curFrame.y >= m_tFrame.f2maxFrame.y)
			{
				m_tFrame.f2curFrame.x = m_tFrame.f2maxFrame.x - 1;
				m_tFrame.f2curFrame.y = m_tFrame.f2maxFrame.y - 1;

				if (m_tFrame.isEndbyCnt)
				{
					++m_tFrame.iCurCnt;
					if (m_tFrame.iCurCnt > m_tFrame.iPlayCnt)
					{
						m_tFrame.f2curFrame.x = 0.f;
						m_tFrame.f2curFrame.y = 0.f;
						m_tFrame.fFrameAcc = 0.f;
						m_tFrame.iCurCnt = 0;
						m_fAccTime = 0.f;
						//m_IsEnable = false;			// > ³¡³»±â
						m_isPlay = false;
					}
						
				}
			}
		}
	}
	Mat->MatTransform._41 = m_tFrame.f2curFrame.x / m_tFrame.f2maxFrame.x;
	Mat->MatTransform._42 = m_tFrame.f2curFrame.y / m_tFrame.f2maxFrame.y;

	Mat->MatTransform(0, 0) = 1 / m_tFrame.f2maxFrame.x;
	Mat->MatTransform(1, 1) = 1 / m_tFrame.f2maxFrame.y;
}

void ChangeUI::Scaling(float x, float y)
{
	scale.x += x;
	scale.y += y;

	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, 1.f);
	XMStoreFloat4x4(&World, matScale);
	World._41;
}

void ChangeUI::GetScale(float * x, float * y)
{
	x = &scale.x;
	y = &scale.y;
}

void ChangeUI::Free()
{
}
