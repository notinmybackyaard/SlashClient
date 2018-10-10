#include "stdafx.h"
#include "DynamicMeshSingle.h"



HRESULT DynamicMeshSingle::Initialize(vector<pair<const string, const string>>& pFilePath)
{
	AnimInfo tAnimInfo;

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;
	bool countStart = false;


	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (int filePath = 0; filePath < pFilePath.size(); ++filePath) // Idle, Walk... 애니메이션 읽어오기
	{
		iAnimframe = 0;
		std::ifstream fin(pFilePath[filePath].second, ios::binary);

		if (!fin)
		{
			return E_FAIL;
		}

		while (!fin.eof()) // 하나의 애니메이션에 대해 읽어오는 부분
		{
			fin >> ignore;

			if (ignore == "*TIMEVALUE" && countStart)
			{
				iAnimframe++;
				//정점 좌표 읽어오기
				Character curCharacter;
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

				for (int i = 0; i < curCharacter.iNumIndex; ++i)
				{
					fin >> ignore; //*MESH_FACE 
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
					fin >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore;
				}

				//텍스쳐 좌표 읽어오기
				fin >> ignore; // }
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

					if (filePath == 0) // Idle만
					{
						vMin = XMVectorMin(vMin, P);
						vMax = XMVectorMax(vMax, P);
					}
				}

				tAnimInfo.AnimationModel.push_back(curCharacter);
			}
			if (ignore == "*MESH_ANIMATION")
			{
				countStart = true;
			}
		}

		tAnimInfo.iAnimationFrameSize = int(tAnimInfo.AnimationModel.size());
		vecAnimFrame.push_back(iAnimframe);

	}



	///////////////////////////////////////////////////////////////////////////////////////

	std::vector<Vertex> vertices;
	std::vector<std::int32_t> indices;


	//Idle 애니메이션 정점, 인덱스 초기화
	int nVertexoffset = 0;
	int nIndexoffset = 0;

	//Idle 버텍스 / 인덱스 오프셋 셋팅
	auto iter = tAnimInfo.AnimationModel.begin();
	auto iter_end = tAnimInfo.AnimationModel.end();

	for (iter; iter != iter_end; ++iter)
	{
		Vertexoffset.push_back(nVertexoffset);
		nVertexoffset += int((*iter).uv.size());

		Indexoffset.push_back(nIndexoffset);
		nIndexoffset += (*iter).iNumIndex * 3;
	}

	//Idle 애니메이션 버텍스 넣음
	iter = tAnimInfo.AnimationModel.begin();
	for (iter; iter != iter_end; ++iter)
	{
		for (int i = 0; i < (*iter).realvecVertex/*vecVertex*/.size(); ++i)
		{
			vertices.push_back((*iter).realvecVertex/*vecVertex*/[i]);
		}
	}

	//Idle 인덱스 셋팅
	int iSize = (int)tAnimInfo.AnimationModel.size();

	for (int i = 0; i < iSize; ++i)
	{
		indices.insert(indices.end(), std::begin(tAnimInfo.AnimationModel[i].uvIndex/*vecIndex*/), std::end(tAnimInfo.AnimationModel[i].uvIndex/*vecIndex*/));
	}


	int nAnimVertexoffset = 0;
	int nAnimIndexoffset = 0;

	//애니메이션 오프셋 셋팅
	for (int i = 0; i < pFilePath.size(); ++i)
	{
		auto iter = tAnimInfo.AnimationModel.begin();
		VertexAnimoffset.push_back(nAnimVertexoffset);
		int vertexSize = (*iter).iNumTexCnt;
		int indexSize = (*iter).iNumTexIndex;
		nAnimVertexoffset += vecAnimFrame[i] * vertexSize;

		IndexAnimoffset.push_back(nAnimIndexoffset);
		nAnimIndexoffset += vecAnimFrame[i] * indexSize;


	}

	int indexTest = int(indices.size());


	BoundingBox bounds;
	XMStoreFloat3(&bounds.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&bounds.Extents, 0.5f*(vMax - vMin));


	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "SingleMeshGeo";

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

	SubmeshGeometry submesh;
	submesh.IndexCount = indexTest;//(UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	submesh.Bounds = bounds;

	geo->DrawArgs["SingleMesh"] = submesh;

	m_Geometry.push_back(std::move(geo));

	return S_OK;
}



void DynamicMeshSingle::Free()
{
}

DynamicMeshSingle * DynamicMeshSingle::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<pair<const string, const string>>& pFilePath)
{
	DynamicMeshSingle* pInstance = new DynamicMeshSingle(d3dDevice);
	if (FAILED(pInstance->Initialize(pFilePath)))
	{
		MSG_BOX(L"DynamicMeshSingle Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * DynamicMeshSingle::Clone(void)
{
	AddRef();
	return this;
}

DynamicMeshSingle::DynamicMeshSingle(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice)
	: Mesh(md3dDevice)
{
}

DynamicMeshSingle::~DynamicMeshSingle()
{
}
