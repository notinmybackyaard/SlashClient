#pragma once

#include "UI.h"

class NumUI : public UI
{
	struct tagNumUI
	{
		InstanceData		instData;
	};
public:
	NumUI(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* wstrUIName, int diffuseSrvHeapIndex);
	~NumUI();
public:
	static NumUI* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* wstrUIName, int diffuseSrvHeapIndex);

	virtual bool			Update(const GameTimer & gt);
	virtual void			SetUI(float size = 0.5f, float moveX = 0.0f, float moveY = 0.0f, float scaleX = 0.0f, float scaleY = 0.0f);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);
	HRESULT					Initialize(wchar_t* uiName);

	void					SetColor(float r, float g, float b, float a);
	XMFLOAT4				GetColor() { return Mat->DiffuseAlbedo; }

	//void					SetMat(XMFLOAT4 color, XMFLOAT4X4);
	void					SetNum(int iNum);

private:
	vector<tagNumUI>				m_vNum;
	Mesh*							m_pMesh[2];
	MeshGeometry*					Geo[2];

private:
	virtual void Free();
};

