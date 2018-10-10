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
	string GetTexName() { return m_strTexName; }

public:
	virtual HRESULT Initialize(vector<pair<const string, const string>> &pFilePath);
	virtual int Update(const GameTimer& gt);
	virtual void Draw();

private:
	string				m_strTexName;

public:
	static StaticMesh* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<pair<const string, const string>> & pFilePath);

	virtual CComponent* Clone(void);
	virtual void Free();

	void BuildBounds();
	//virtual HRESULT LoadMesh(const wchar_t* AnimName, const char* pFilePath);
};

