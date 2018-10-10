#pragma once

#include "Mesh.h"


class UIMesh :
	public Mesh
{
public:
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
public:
	explicit UIMesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);
	~UIMesh();
public:
	HRESULT Initialize(XMFLOAT2 move, XMFLOAT2 scale, float size, float fZ);
	virtual int Update(const GameTimer& gt);
	virtual void Draw();

private:
	XMFLOAT2			m_Move = XMFLOAT2(0,0);
	XMFLOAT2			m_Scale = XMFLOAT2(0, 0);
	float				m_fSize = 0.0f;
	float				m_fZ = 0.f;

public:
	static UIMesh* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, XMFLOAT2 move, XMFLOAT2 scale, float size, float fZ = 0.f);

	virtual CComponent* Clone(void);
	virtual void Free();

	vector <std::unique_ptr<MeshGeometry>>	m_Geometry;
};

