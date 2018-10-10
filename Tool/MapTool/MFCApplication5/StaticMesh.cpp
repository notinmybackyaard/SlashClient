#include "stdafx.h"
#include "StaticMesh.h"
#include "Texture_Manager.h"


StaticMesh::StaticMesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice) : Mesh(d3dDevice)
{

}


StaticMesh::~StaticMesh()
{
}

HRESULT StaticMesh::Initialize(vector<pair<const string, const string>> &pFilePath)
{
	std::ifstream fin(pFilePath[0].second);
	m_strPath = pFilePath[0].second;

	if (!fin)
	{
		return E_FAIL;
	}


	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;
	Character curCharacter;
	float Infinity = FLT_MAX;
	//XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	//XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMFLOAT3 vMinf3(+Infinity, +Infinity, +Infinity);
	XMFLOAT3 vMaxf3(-Infinity, -Infinity, -Infinity);

	vMin = XMLoadFloat3(&vMinf3);

	vMax = XMLoadFloat3(&vMaxf3);


	//애니메이션 갯수 세기
	while (!fin.eof())
	{
		fin >> ignore;
		if (ignore == "*MATERIAL_LIST")
		{
			fin >> ignore;

			while (ignore != "}")
			{
				fin >> ignore;
				if (ignore == "*BITMAP")
				{
					wstring wstrFileName;
					wstring wstrRealFileName;
					fin >> ignore;
					wstrRealFileName.assign(ignore.begin(), ignore.end());
					auto b = ignore.begin();
					auto e = ignore.end();
					fs::path t_Path = wstrFileName.assign(++b, --e);									// > " 빼고 경로 넣음
					wstrFileName = t_Path.filename();													// > TextureName.tga/png/... 파일 이름 가져오기 
					size_t iLength = wstrFileName.length();
					size_t iDotPos = wstrFileName.rfind(L".");						 					// > .tga에서 .위치 찾기
					wstrFileName.erase(iDotPos, iLength);												// > TextureName만 남기기
					//wstring wstrTexPath = L"../../../Textures/" + wstrFileName + L".dds";					// > 경로 지정 L"../../Textures/TextureName.dds"
					wstring wstrTexPath = L"../../../Slash/Chapter 16 Instancing and Frustum Culling/InstancingAndCulling/Assets/Textures/" + wstrFileName + L".dds";
					auto Tex = new Texture;
					string texName;
					Tex->Name = texName.assign(wstrFileName.begin(), wstrFileName.end());
					Tex->Filename = wstrTexPath;
					ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(m_d3dDevice.Get(),
					mCommandList.Get(), Tex->Filename.c_str(),
						Tex->Resource, Tex->UploadHeap));

					m_strTexName = texName;
					wstring wstr = L"";
					wstr.assign(Tex->Name.begin(), Tex->Name.end());
					CTexture_Manager::GetInstance()->Ready_Texture(wstr, Tex, HEAP_DEFAULT);
				}
			}
		}

		if (ignore == "*TIMEVALUE")
		{
			//정점 좌표 읽어오기
			fin >> curCharacter.iTimeValue;
			fin >> ignore; //*MESH_NUMVERTEX 
			fin >> curCharacter.iNumVertex;
			fin >> ignore; //*MESH_NUMFACES 
			fin >> curCharacter.iNumIndex;
			fin >> ignore; //*MESH_VERTEX_LIST 
			fin >> ignore; //{
			for (int i = 0; i < curCharacter.iNumVertex; ++i)
			{
				Vertex curPos;
				fin >> ignore; //*MESH_VERTEX
				fin >> ignore; // Index;
				fin >> curPos.Pos.x;
				fin >> curPos.Pos.y;
				fin >> curPos.Pos.z;

				curCharacter.vecVertex.push_back(curPos);

			}
			//인덱스 읽어오기
			fin >> ignore; //}
			fin >> ignore; //*MESH_FACE_LIST 
			fin >> ignore; // {

			fin >> ignore; //*MESH_FACE 
			for (int i = 0; i < curCharacter.iNumIndex; ++i)
			{
				fin >> ignore; // Index:
				fin >> ignore; // A:
				fin >> ignore;
				curCharacter.vecIndex.push_back(atoi(ignore.c_str()));
				fin >> ignore; // B:
				fin >> ignore;
				curCharacter.vecIndex.push_back(atoi(ignore.c_str()));
				fin >> ignore; // C:
				fin >> ignore;
				curCharacter.vecIndex.push_back(atoi(ignore.c_str()));
				while (ignore != "*MESH_FACE" && ignore != "}")
					fin >> ignore;
			}

			//텍스쳐 좌표 읽어오기
			fin >> ignore; // *MESH_NUMTVERTEX 
			fin >> curCharacter.iNumTexCnt;
			fin >> ignore; // *MESH_TVERLIST
			fin >> ignore; // {
			for (int i = 0; i < curCharacter.iNumTexCnt; ++i)
			{
				XMFLOAT2 curUV;

				fin >> ignore; // *MESH_TVERT
				fin >> ignore; // Index;
				fin >> curUV.x;
				fin >> curUV.y;
				fin >> ignore; // w

				curUV.y = 1.0f - curUV.y;

				curCharacter.uv.push_back(curUV);
			}

			//텍스쳐 좌표 인덱스 읽어오기
			fin >> ignore; // }
			fin >> ignore;

			fin >> curCharacter.iNumTexIndex;
			fin >> ignore; // *MESH_TFACELIST
			fin >> ignore; // {
			for (int i = 0; i < curCharacter.iNumTexIndex; ++i)
			{
				fin >> ignore; // *MESH_TFACE
				fin >> ignore; // INDEX
				fin >> ignore;
				curCharacter.uvIndex.push_back(atoi(ignore.c_str()));
				fin >> ignore;
				curCharacter.uvIndex.push_back(atoi(ignore.c_str()));
				fin >> ignore;
				curCharacter.uvIndex.push_back(atoi(ignore.c_str()));
			}


			while (ignore != "*MESH_NORMALS")
			{
				fin >> ignore; // 계속읽어

			}
			if (ignore == "*MESH_NORMALS")
			{

				fin >> ignore; //{
				for (int i = 0; i < curCharacter.iNumIndex; ++i)
				{
					int index = 0;

					fin >> ignore; //*MESH_FACENORMAL 
					fin >> ignore; // FaceIndex

					atoi(ignore.c_str());
					fin >> ignore >> ignore >> ignore;

					for (int k = 0; k < 3; ++k)
					{
						fin >> ignore; //*MESH_VERTEXNORMAL 
						fin >> index; // index;
						fin >> curCharacter.vecVertex[index].Normal.x;
						fin >> curCharacter.vecVertex[index].Normal.y;
						fin >> curCharacter.vecVertex[index].Normal.z;

					}


				}
			}
			curCharacter.realvecVertex.resize(curCharacter.uv.size());

			for (int i = 0; i < curCharacter.uv.size(); ++i)
			{
				curCharacter.realvecVertex[i].TexC = curCharacter.uv[i];
			}
			for (int i = 0; i < curCharacter.vecIndex.size(); ++i)
			{
				int vertexIndex = curCharacter.vecIndex[i];
				int realvecVertexIndex = curCharacter.uvIndex[i];

				curCharacter.realvecVertex[realvecVertexIndex].Pos = curCharacter.vecVertex[vertexIndex].Pos;
				curCharacter.realvecVertex[realvecVertexIndex].Normal = curCharacter.vecVertex[vertexIndex].Normal;
				XMVECTOR P = XMLoadFloat3(&curCharacter.realvecVertex[realvecVertexIndex].Pos);

				vMin = XMVectorMin(vMin, P);
				vMax = XMVectorMax(vMax, P);
			}
			break;
		}

	}
	fin.close();

	//m_mapAnimations[const_cast<wchar_t*>(AnimName)] = tAnimInfo;


	///////////////////////////////////////////////////////////////////////////////////////


	//std::vector<Vertex> vertices;
	//std::vector<std::int32_t> indices;



	//Idle 버텍스 / 인덱스 오프셋 셋팅



	//Idle 애니메이션 버텍스 넣음


	//for (int i = 0; i < curCharacter.realvecVertex/*vecVertex*/.size(); ++i)
	//{
	//	vertices.push_back(curCharacter.realvecVertex/*vecVertex*/[i]);
	//}

	//Idle 인덱스 셋팅

	//indices = curCharacter.uvIndex;


	BoundingBox bounds;
	XMStoreFloat3(&bounds.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&bounds.Extents, 0.5f*(vMax - vMin));



	size_t indexTest = curCharacter.uvIndex.size();

	const UINT vbByteSize = (UINT)curCharacter.realvecVertex.size() * sizeof(Vertex);

	const UINT ibByteSize = (UINT)curCharacter.uvIndex.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "BarrelGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), curCharacter.realvecVertex.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), curCharacter.uvIndex.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), curCharacter.realvecVertex.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), curCharacter.uvIndex.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)curCharacter.uvIndex.size();//(UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	submesh.Bounds = bounds;

	geo->DrawArgs["Barrel"] = submesh;

	m_Geometry.push_back(std::move(geo));

	BuildBounds();


	return S_OK;
}

int StaticMesh::Update(const GameTimer & gt)
{
	return 0;
}

void StaticMesh::Draw()
{
}

void StaticMesh::Free()
{
}

StaticMesh * StaticMesh::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<pair<const string, const string>> & pFilePath)
{
	StaticMesh* pInstance = new StaticMesh(d3dDevice);
	if (FAILED(pInstance->Initialize(pFilePath)))
	{
		MSG_BOX(L"StaticMesh Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * StaticMesh::Clone(void)
{
	AddRef();
	return this;
}

void StaticMesh::BuildBounds()
{
	BoundingBox bounds;
	XMStoreFloat3(&bounds.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&bounds.Extents, 0.5f*(vMax - vMin));

	/////////////////////// 바운딩박스 버텍스버퍼뷰

	std::vector<Vertex> vertices;
	std::vector<std::int32_t> indices;

	Vertex v[24];

	XMFLOAT3 xmfVMin;
	XMFLOAT3 xmfVMax;

	XMStoreFloat3(&xmfVMin, vMin);
	XMStoreFloat3(&xmfVMax, vMax);

	float w2 = 0.5f * (xmfVMax.x - xmfVMin.x);
	float h2 = 0.5f*(xmfVMax.y - xmfVMin.y);
	float d2 = 0.5f * (xmfVMax.z - xmfVMin.z);

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2 + bounds.Center.x, -h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2 + bounds.Center.x, +h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2 + bounds.Center.x, +h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2 + bounds.Center.x, -h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.					 						
	v[4] = Vertex(-w2 + bounds.Center.x, -h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2 + bounds.Center.x, -h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2 + bounds.Center.x, +h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2 + bounds.Center.x, +h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2 + bounds.Center.x, +h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2 + bounds.Center.x, +h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2 + bounds.Center.x, +h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2 + bounds.Center.x, +h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2 + bounds.Center.x, -h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2 + bounds.Center.x, -h2 + bounds.Center.y, -d2 + bounds.Center.z, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2 + bounds.Center.x, -h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2 + bounds.Center.x, -h2 + bounds.Center.y, +d2 + bounds.Center.z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.					 						
	v[16] = Vertex(-w2 + bounds.Center.x, -h2 + bounds.Center.y, +d2 + bounds.Center.z, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2 + bounds.Center.x, +h2 + bounds.Center.y, +d2 + bounds.Center.z, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2 + bounds.Center.x, +h2 + bounds.Center.y, -d2 + bounds.Center.z, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2 + bounds.Center.x, -h2 + bounds.Center.y, -d2 + bounds.Center.z, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.											
	v[20] = Vertex(+w2 + bounds.Center.x, -h2 + bounds.Center.y, -d2 + bounds.Center.z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2 + bounds.Center.x, +h2 + bounds.Center.y, -d2 + bounds.Center.z, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2 + bounds.Center.x, +h2 + bounds.Center.y, +d2 + bounds.Center.z, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2 + bounds.Center.x, -h2 + bounds.Center.y, +d2 + bounds.Center.z, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	int index[36];
	// Fill in the front face index data
	index[0] = 0; index[1] = 1; index[2] = 2;
	index[3] = 0; index[4] = 2; index[5] = 3;

	// Findexll indexn the back face indexndex data
	index[6] = 4; index[7] = 5; index[8] = 6;
	index[9] = 4; index[10] = 6; index[11] = 7;

	// Findexll indexn the top face indexndex data
	index[12] = 8; index[13] = 9; index[14] = 10;
	index[15] = 8; index[16] = 10; index[17] = 11;

	// Findexll indexn the bottom face indexndex data
	index[18] = 12; index[19] = 13; index[20] = 14;
	index[21] = 12; index[22] = 14; index[23] = 15;

	// Findexll indexn the left face indexndex data
	index[24] = 16; index[25] = 17; index[26] = 18;
	index[27] = 16; index[28] = 18; index[29] = 19;

	// Findexll indexn the rindexght face indexndex data
	index[30] = 20; index[31] = 21; index[32] = 22;
	index[33] = 20; index[34] = 22; index[35] = 23;

	vertices.assign(&v[0], &v[24]);
	indices.assign(&index[0], &index[36]);

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "BarrelgeoBounds";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh2;
	submesh2.IndexCount = (UINT)indices.size();//(UINT)indices.size();
	submesh2.StartIndexLocation = 0;
	submesh2.BaseVertexLocation = 0;
	submesh2.Bounds = bounds;

	geo->DrawArgs["BarrelBounds"] = submesh2;

	m_Geometry.push_back(std::move(geo));
}

