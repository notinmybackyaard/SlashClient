#pragma once

#include "UI.h"

class HPBar
	: public UI
{

public:
	HPBar(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT2 _move, XMFLOAT2 _scale, float _size, int diffuseSrvHeapIndex, float fZ);
	~HPBar();
public:
	static HPBar* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT2 move, XMFLOAT2 scale, float size, int diffuseSrvHeapIndex, float fZ = 0.f);

	virtual bool			Update(const GameTimer & gt);
	virtual void			SetUI(float _size = 0.5f, float _moveX = 0.0f, float _moveY = 0.0f, float _scaleX = 0.0f, float _scaleY = 0.0f);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);
	virtual HRESULT			Initialize(XMFLOAT2 move, XMFLOAT2 scale, float size);

	void GetUIValue(float* _size , float* _moveX, float* _moveY, float* _scaleX, float* _scaleY);

	float&	GetMax() { return m_fMax; }
	float&	GetCur() { return m_fCur; }

	void	SetColor(float r, float g, float b, float a) { Mat->DiffuseAlbedo = XMFLOAT4(r, g, b, a); }
	void	SetAlpha(float a) { Mat->DiffuseAlbedo.w = a; }
	float	GetAlpha() { return Mat->DiffuseAlbedo.w; }

	static unsigned long	m_iAllBarUIIndex;
	unsigned long			m_iMyUIID;

private:
	float					m_fMax = INIT_PLAYER_HP;
	float					m_fCur = INIT_PLAYER_HP;

	float					m_fZ;

private:
	virtual void Free();
};



