#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice)
	: m_d3dDevice(d3dDevice)
{
}

Mesh::~Mesh()
{
}

HRESULT Mesh::Initialize(vector<pair<const string, const string>>  &pFilePath)
{
	return S_OK;
}

HRESULT Mesh::Initialize()
{
	return E_NOTIMPL;
}

int Mesh::Update(const GameTimer & gt)
{
	return 0;
}

void Mesh::Render()
{
}

void Mesh::Free()
{
}
