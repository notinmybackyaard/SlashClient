#pragma once
#include "Component.h"
#include "GameTimer.h"
#include "Define.h"

class Mesh
	: public CComponent
{
public:
	enum MESHTYPE { MESH_STATIC, MESH_STATIC_INST, MESH_DYNAMIC, MESH_DYNAMIC_SING };
public:
	XMVECTOR vMin;

	XMVECTOR vMax;
public:
	typedef struct character
	{
		int iTimeValue = 0;
		vector<Vertex> vecVertex;
		vector<int> vecIndex;

		int iNumVertex = 0;
		int iNumIndex = 0;

		vector<XMFLOAT2> uv;
		vector<int>		uvIndex;

		int iNumTexCnt = 0;
		int iNumTexIndex = 0;

		vector<Vertex> realvecVertex;

	}Character;

public:
	explicit Mesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);
	virtual ~Mesh();

public:
	const string& GetMeshPath() { return m_strPath; }

public:
	virtual HRESULT Initialize();
	//virtual HRESULT Initialize(const string& pFilePath);
	virtual HRESULT Initialize(vector<pair<const string, const string>> & pFilePath);
	virtual int Update(const GameTimer& gt);
	virtual void Render();
	virtual void Free();

public:
	virtual CComponent* Clone() = 0;
public:
	//virtual HRESULT LoadMesh(const wchar_t* AnimName, const char* pFilePath) = 0;
	
protected:
	Microsoft::WRL::ComPtr<ID3D12Device>		m_d3dDevice;
	string					m_strPath;
};