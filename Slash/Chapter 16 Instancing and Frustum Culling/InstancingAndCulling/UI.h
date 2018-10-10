#pragma once

#include "GameObject.h"


class UI : public CGameObject
{
protected:
	XMFLOAT2 move = XMFLOAT2(0, 0);
	XMFLOAT2 scale = XMFLOAT2(0, 0);
	float size = 0;

	int m_iDiffuseSrvHeapIndex = 0;
public:
	UI(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize);
	virtual ~UI();
public:
	virtual bool			Update(const GameTimer & gt);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);
	virtual HRESULT			Initialize(XMFLOAT2 move, XMFLOAT2 scale, float size);
public:

	static UI* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT2 move, XMFLOAT2 scale, float size, int diffuseSrvHeapIndex);

private:
	virtual void			Free();
};

