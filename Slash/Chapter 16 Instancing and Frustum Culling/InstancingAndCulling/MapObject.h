#pragma once
#include "GameObject.h"

class CMapObject
	: public CGameObject
{
private:
	explicit CMapObject(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t* meshName);
public:
	virtual ~CMapObject();

public:
	virtual HRESULT Initialize();
	virtual bool	Update(const GameTimer& gt);
	virtual void	Render(ID3D12GraphicsCommandList* cmdList);

public:
	void SetTexture(int idx) { Mat->DiffuseSrvHeapIndex = idx; }
	//void SetAlpha(bool)

public:
	static CMapObject* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, wchar_t* meshName);

private:
	wchar_t*				m_wstrMeshName;
	bool					m_IsAlpha;

public:
	virtual void Free();
};