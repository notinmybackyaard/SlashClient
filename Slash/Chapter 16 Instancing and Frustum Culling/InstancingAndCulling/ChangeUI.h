#pragma once

#include "UI.h"
#include "Define.h"

class ChangeUI : public UI
{

public:
	ChangeUI(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT2 _move, XMFLOAT2 _scale, float _size, int diffuseSrvHeapIndex, bool isCon, float fZ = 0, float fStartTime = 0.f);
	ChangeUI(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* pMeshName, int diffuseSrvHeapIndex, bool isCon);
	~ChangeUI();
public:
	static ChangeUI* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, XMFLOAT2 move, XMFLOAT2 scale, float size, int diffuseSrvHeapIndex, bool isCon, float fZ = 0, float fStartTime = 0.f);
	static ChangeUI* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* pMeshName, int diffuseSrvHeapIndex, bool isCon);

	virtual bool			Update(const GameTimer & gt);
	virtual void			SetUI(float size = 0.5f, float moveX = 0.0f, float moveY = 0.0f, float scaleX = 0.0f, float scaleY = 0.0f);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);
	virtual HRESULT			Initialize(XMFLOAT2 move, XMFLOAT2 scale, float size);
	virtual HRESULT			Initialize(wchar_t* pMeshName, int diffuseSrvHeapIndex);

public:
	void		SetisChange(bool _isChange);
	void		SetChangeInfo(XMFLOAT4 _xm, float _time);
	
	void		SetFrameInfo(UV_FRAME_INFO info);
	void		MoveFrame(const GameTimer& gt);

	void		SetPlay(bool _end) { m_isPlay = _end; if (m_isPlay == true) m_isEnd = false; }

	void		Scaling(float x, float y);
	void		GetScale(float* x, float* y);

	void		SetInitColor(XMFLOAT4 color) { m_xmInitColor = color; }
	void		SetColor(XMFLOAT4 color) { Mat->DiffuseAlbedo = color; }
	void		SetAlpha(float a) { Mat->DiffuseAlbedo.w = a; }
	void		SetRemain(bool remain) { m_isRemain = remain; }

private:
	bool				m_IsContinue = false;
	bool				m_IsChange = false;
	bool				m_ChangeValue = true;
	XMFLOAT4			m_xmf4ColorChange;
	XMFLOAT4			m_ChangeColor;
	float				m_fChangeTime = 0.f;
	float				m_fTimeAccc = 0.f;
	float				m_fZ = 0.f;
	UV_FRAME_INFO		m_tFrame;
	bool				m_isFrame = false;
	float				m_fStartTime = 0.f;
	float				m_fAccTime = 0.f;
	bool				m_isPlay = false;
	bool				m_isRemain = false;
	bool				m_isEnd = false;
	XMFLOAT4			m_xmInitColor;

private:
	virtual void Free();
};

