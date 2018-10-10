#pragma once
#include "GameObject.h"
#include "Define.h"

class CSkill_Billboard
	: public CGameObject
{
private:
	explicit CSkill_Billboard(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string TexName);
	virtual ~CSkill_Billboard();

public:
	virtual HRESULT Initialize();
	virtual bool Update(const GameTimer& gt);
	virtual void Render(ID3D12GraphicsCommandList* cmdList);

public:
	static CSkill_Billboard* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string TexName);

private:
	string			m_strTexName;

private:
	virtual void Free();
};