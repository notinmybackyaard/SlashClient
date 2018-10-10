#pragma once

#include "Scene.h"

class SelectPlayer;
//class ChangeUI;

class CSelectScene
	: public CScene
{
private:
	explicit CSelectScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize);
	virtual ~CSelectScene();

public:
	virtual HRESULT Initialize();
	virtual bool	Update(const GameTimer& gt);
	virtual void	Render(ID3D12GraphicsCommandList* cmdList);

private:
	vector<SelectPlayer*>		m_vecSelect;
	//vector<ChangeUI*>		m_vecSelect;
	bool					m_isLeft = true;
	bool					m_isSelect = false;
	float					m_fSelectTime = 0.f;
public:
	static CSelectScene* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize);

private:
	virtual void Free();
};