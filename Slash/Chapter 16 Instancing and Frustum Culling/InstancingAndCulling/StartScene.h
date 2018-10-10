#pragma once

#include "Scene.h"

//class SelectPlayer;
class ChangeUI;

class CStartScene
	: public CScene
{
private:
	explicit CStartScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize);
	virtual ~CStartScene();

public:
	virtual HRESULT Initialize();
	virtual bool	Update(const GameTimer& gt);
	virtual void	Render(ID3D12GraphicsCommandList* cmdList);

private:
	//vector<SelectPlayer*>		m_vecSelect;
	vector<ChangeUI*>		m_vecSelect;
	bool					m_isLeft = true;
	float					m_fChangeTime = 0.f;
public:
	static CStartScene* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize);

private:
	virtual void Free();
};