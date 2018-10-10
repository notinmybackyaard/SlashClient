#pragma once
#include "GameObject.h"
#include "Define.h"

class CEffect;

class CSkillEffect
	: public CGameObject
{
	explicit CSkillEffect(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name);
	explicit CSkillEffect(CSkillEffect& other);
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
	void					Set_ParentMatrix(XMFLOAT4X4* f4x4Parent);
	void					Set_RotParentMatirx(XMFLOAT4X4* f4x4Parent, float fRot);
	void					Set_Parent(CGameObject* pObj);
	void					Set_IsCon(bool iscon);

private:
	list<CEffect*>			m_EffectList;
	string					m_strName;
	bool					m_isPlay = false;
	CGameObject*			m_pParent = nullptr;
	bool					m_isContinue = false;

public:
	static CSkillEffect* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap>& srv, UINT srvSize, string name);
	static CSkillEffect* Create(CSkillEffect& other);

private:
	virtual void Free();
};