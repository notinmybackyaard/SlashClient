#pragma once

#include "Mesh.h"

class StaticMesh : public Mesh
{

public:
	vector<std::unique_ptr<MeshGeometry>>	m_Geometry;

public:
	explicit StaticMesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);
	virtual ~StaticMesh();
public:
	HRESULT Initialize(vector<pair<const string, const string>> &pFilePath, bool isInst);
	virtual int Update(const GameTimer& gt);
	virtual void Draw();


public:
	static StaticMesh* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<pair<const string, const string>> & pFilePath, bool isInst = true);

	virtual CComponent* Clone(void);
	virtual void Free();

	void BuildBounds();
	//virtual HRESULT LoadMesh(const wchar_t* AnimName, const char* pFilePath);
};

