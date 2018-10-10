#pragma once

#include "GameObject.h"
#include "Define.h"

class CEffect
	: public CGameObject
{
private:
	explicit CEffect(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, EFFECT_INFO info);
	explicit CEffect(CEffect& other);
	virtual ~CEffect();

public:
	EFFECT_INFO&		Get_EffectInfo() { return m_tInfo; }
	UV_FRAME_INFO&		Get_FrameInfo() { return m_tFrame; }

	bool				Get_IsFrame() { return m_IsFrame; }

public:
	virtual HRESULT Initialize();
	virtual bool	Update(const GameTimer& gt);
	virtual void	Render(ID3D12GraphicsCommandList* cmdList);

public:
	void	Update_Default(const GameTimer& gt);
	void	Update_Play(const GameTimer& gt);
	void	Update_Billboard(const GameTimer& gt);
	void	SetTexture(Texture* tex);
	void	SetTexture(string texName);
	void	SetTexture(int idx) { Mat->DiffuseSrvHeapIndex = idx; }
	void	SetPlay(bool _isPlay);
	void	SetIsFrame(bool _isFrame);
	void	MoveFrame(const GameTimer& gt);
	void	SetIsCon(bool isC) { m_IsCon = isC; }
public:
	static CEffect* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, EFFECT_INFO info);
	static CEffect*	Create(CEffect& other);

private:
	EFFECT_INFO		m_tInfo;
	UV_FRAME_INFO	m_tFrame;

	XMFLOAT3		m_ChangePos;
	XMFLOAT3		m_ChangeSize;
	XMFLOAT3		m_ChangeRot;
	XMFLOAT4		m_ChangeColor;

	float			m_fTimeDeltaAcc = 0.f;
	float			m_fLifeTimeAcc = 0.f;

	bool			m_IsPlay = false;
	bool			m_IsFrame;
	bool			m_IsCon = false;

private:
	virtual void Free();
};