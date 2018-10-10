#include "stdafx.h"
#include "UIMesh.h"




UIMesh::UIMesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice) : Mesh(d3dDevice)
{
}

UIMesh::~UIMesh()
{
}

HRESULT UIMesh::Initialize(XMFLOAT2 move, XMFLOAT2 scale, float size, float fZ)
{
	std::vector<Vertex> vertices(6);
	//std::vector<uint32> Indices32;


	vertices[0].Pos = XMFLOAT3((-size + move.x) * scale.x, (size + move.y) * scale.y, fZ);
	vertices[0].TexC = XMFLOAT2(0.0f, 0.0f);

	vertices[1].Pos = XMFLOAT3((size + move.x)* scale.x, (-size + move.y) * scale.y, fZ);
	vertices[1].TexC = XMFLOAT2(1.0f, 1.0f);

	vertices[2].Pos = XMFLOAT3((-size + move.x)* scale.x, (-size + move.y) * scale.y, fZ);
	vertices[2].TexC = XMFLOAT2(0.0f, 1.0f);



	vertices[3].Pos = XMFLOAT3((-size + move.x)* scale.x, (size + move.y) * scale.y, fZ);
	vertices[3].TexC = XMFLOAT2(0.0f, 0.0f);

	vertices[4].Pos = XMFLOAT3((size + move.x)* scale.x, (size + move.y) * scale.y, fZ);
	vertices[4].TexC = XMFLOAT2(1.0f, 0.0f);

	vertices[5].Pos = XMFLOAT3((size + move.x)* scale.x, (-size + move.y) * scale.y, fZ);
	vertices[5].TexC = XMFLOAT2(1.0f, 1.0f);

	/*
	//uint32 m = 2;
	//uint32 n = 2;
	//float width = 1.0f;
	//float depth = 1.0f;

	//uint32 vertexCount = m * n;
	//uint32 faceCount = (m - 1)*(n - 1) * 2;

	////
	//// Create the vertices.
	////

	//float halfWidth = 0.5f*width;
	//float halfDepth = 0.5f*depth;

	//float dx = width / (n - 1);
	//float dz = depth / (m - 1);

	//float du = 1.0f / (n - 1);
	//float dv = 1.0f / (m - 1);

	//for (uint32 i = 0; i < m; ++i)
	//{
	//	float z = halfDepth - i * dz;
	//	for (uint32 j = 0; j < n; ++j)
	//	{
	//		float x = -halfWidth + j * dx;

	//		vertices[i*n + j].Pos = XMFLOAT3(x, z, 0.0f);
	//		vertices[i*n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	//		// Stretch texture over grid.
	//		vertices[i*n + j].TexC.x = j * du;
	//		vertices[i*n + j].TexC.y = i * dv;
	//	}
	//}

	////
	//// Create the indices.
	////

	//Indices32.resize(faceCount * 3); // 3 indices per face

	//										  // Iterate over each quad and compute indices.
	//uint32 k = 0;
	//for (uint32 i = 0; i < m - 1; ++i)
	//{
	//	for (uint32 j = 0; j < n - 1; ++j)
	//	{
	//		Indices32[k] = i * n + j;
	//		Indices32[k + 1] = i * n + j + 1;
	//		Indices32[k + 2] = (i + 1)*n + j;

	//		Indices32[k + 3] = (i + 1)*n + j;
	//		Indices32[k + 4] = i * n + j + 1;
	//		Indices32[k + 5] = (i + 1)*n + j + 1;

	//		k += 6; // next quad
	//	}
	//}
*/

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	//const UINT ibByteSize = (UINT)Indices32.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	//ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	//CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), Indices32.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	//geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
	//	mCommandList.Get(), Indices32.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	//geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	//submesh.IndexCount = Indices32.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["UI"] = submesh;


	m_Geometry.push_back(std::move(geo));

	return S_OK;

}

int UIMesh::Update(const GameTimer & gt)
{
	return 0;
}

void UIMesh::Draw()
{
}

UIMesh * UIMesh::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, XMFLOAT2 move, XMFLOAT2 scale, float size, float fZ)
{
	UIMesh* pInstance = new UIMesh(d3dDevice);
	if (pInstance->Initialize(move,scale,size, fZ))
	{
		MSG_BOX(L"UIMesh Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * UIMesh::Clone(void)
{
	AddRef();
	return this;
}

void UIMesh::Free()
{
}
