#pragma once

#include "GameObject.h"

class GameTimer;

class CSkill
	: public CGameObject
{
private:
	explicit CSkill(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize);
	virtual ~CSkill();

public:
	virtual HRESULT		Initialize();
	virtual bool		Update(const GameTimer& gt);
	virtual void		Render(ID3D12GraphicsCommandList* cmdList);

public:
	static CSkill* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize);

private:
	virtual void Free();
};