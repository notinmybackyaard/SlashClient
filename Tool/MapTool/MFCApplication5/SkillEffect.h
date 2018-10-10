#pragma once
#include "GameObject.h"
#include "Define.h"

class CEffect;

class CSkillEffect
	: public CGameObject
{
	explicit CSkillEffect(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name);
	virtual ~CSkillEffect();

public:
	virtual HRESULT Initialize();
	virtual bool	Update(const GameTimer& gt);
	virtual void	Render(ID3D12GraphicsCommandList* cmdList);

public:
	list<CEffect*>&			GetEffectList() { return m_EffectList; }
	HRESULT					AddEffect(CEffect* effect) { if (!effect) return E_FAIL; m_EffectList.push_back(effect); return S_OK; }
	string					GetName() { return m_strName; }
	void					SetPlay(bool _isPlay);
	void					Set_Enable(bool isEnable);

private:
	list<CEffect*>			m_EffectList;
	string					m_strName;
	bool					m_isPlay = false;

public:
	static CSkillEffect* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name);

private:
	virtual void Free();
};