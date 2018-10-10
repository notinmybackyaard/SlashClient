#pragma once

#include "GameObject.h"

class GameTimer;

class CBoundingBox
	: public CGameObject
{
private:
	explicit CBoundingBox(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT3& f3Pos, XMFLOAT3& f3Scale);
public:
	virtual ~CBoundingBox();

public:
	virtual HRESULT Initialize();
	virtual bool Update(const GameTimer& gt);
	virtual void Render(ID3D12GraphicsCommandList* cmdList);

	//virtual void	SetPosition(float x, float y, float z);
	//virtual void	SetPosition(XMFLOAT3 xmf3Position);

private:
	XMFLOAT3		m_f3Scale;
	XMFLOAT3		m_f3Position;

public:
	static CBoundingBox* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT3& f3Pos, XMFLOAT3& f3Scale);

private:
	virtual void Free();
};