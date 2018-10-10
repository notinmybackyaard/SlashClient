#include "stdafx.h"
#include "BasicLine.h"

CBasicLine::CBasicLine(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
	: CGameObject(d3dDevice, srv, srvSize)
{
}

CBasicLine::~CBasicLine()
{
}

HRESULT CBasicLine::Initialize()
{
	return S_OK;
}

bool CBasicLine::Update(const GameTimer & gt)
{
	return false;
}

void CBasicLine::Render(ID3D12GraphicsCommandList * cmdList)
{
}

CBasicLine * CBasicLine::Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize)
{
	CBasicLine* pInstance = new CBasicLine(d3dDevice, srv, srvSize);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"CBasicLine Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBasicLine::Free()
{
}
