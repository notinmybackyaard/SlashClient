#pragma once
#include "Mesh.h"

class CBillboardMesh
	: public Mesh
{
public:
	struct TreeSpriteVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};
private:
	explicit CBillboardMesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);
	virtual ~CBillboardMesh();

public:
	MeshGeometry* Get_Geometry() { return m_Geometry.get(); }

public:
	HRESULT Initialize();
	virtual int Update(const GameTimer& gt);
	virtual void Draw();

private:
	std::unique_ptr<MeshGeometry>		m_Geometry;

public:
	static CBillboardMesh* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);

public:
	virtual CComponent* Clone();

private:
	virtual void Free();
};