#include "stdafx.h"
#include "BillboardMesh.h"

CBillboardMesh::CBillboardMesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice)
	: Mesh(d3dDevice)
{

}

CBillboardMesh::~CBillboardMesh()
{
}

HRESULT CBillboardMesh::Initialize()
{
	//static const int treeCount = 16;
	//std::array<TreeSpriteVertex, 16> vertices;
	//for (UINT i = 0; i < treeCount; ++i)
	//{
	//	float x = MathHelper::RandF(-45.0f, 45.0f);
	//	float z = MathHelper::RandF(-45.0f, 45.0f);
	//	float y = 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));

	//	// Move tree slightly above land height.
	//	y += 8.0f;

	//	vertices[i].Pos = XMFLOAT3(x, y, z);
	//	vertices[i].Size = XMFLOAT2(20.0f, 20.0f);	
	//}
	std::array<TreeSpriteVertex, 1> vertices;
	vertices[0].Pos = XMFLOAT3(10, 10, 0);
	vertices[0].Size = XMFLOAT2(20.0f, 20.0f);

	std::array<std::uint16_t, 1> indices =
	{
		0
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "BillboardGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(TreeSpriteVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["points"] = submesh;

	m_Geometry = std::move(geo);
	return S_OK;
}

int CBillboardMesh::Update(const GameTimer & gt)
{
	return 0;
}

void CBillboardMesh::Draw()
{
}

CBillboardMesh * CBillboardMesh::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice)
{
	CBillboardMesh* pInstance = new CBillboardMesh(d3dDevice);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"CBillboardMesh Created Failed");
	}
	return pInstance;
}

CComponent * CBillboardMesh::Clone()
{
	AddRef();

	return this;
}

void CBillboardMesh::Free()
{
}
