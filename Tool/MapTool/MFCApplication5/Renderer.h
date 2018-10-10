#pragma once

#include "Component.h"

class CGameTimer;
class CGameObject;

class CRenderer : public CComponent
{
public:
	enum RenderType { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONALPHA_FORWARD, RENDER_NONALPHA_INSTANCING, RENDER_NONALPHA_DEFFERD, RENDER_ALPHA, RENDER_ALPHA_INST, RENDER_ALPHA_DEFAULT, RENDER_UI, RENDER_FADE, RENDER_PREVIEW, RENDER_END};

private:
	explicit CRenderer(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize);
public:
	virtual ~CRenderer();

public:
	HRESULT Add_RenderGroup(RenderType type, CGameObject* pObj);

	void SetPSOs(const std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>& PSOs) { mPSOs = PSOs; }

	vector<ComPtr<ID3D12DescriptorHeap>>		GetSrvDescriptorHeap() { return mSrvDescriptorHeap; }
	UINT&										GetCbvSrvDescriptorSize() { return mCbvSrvDescriptorSize; }

public:
	HRESULT				Initialize();
	virtual void		Update_Component(const GameTimer& gt);
	void				Render(ID3D12GraphicsCommandList* cmdList);
	void				Render_Priority(ID3D12GraphicsCommandList* cmdList);
	void				Render_ForWard(ID3D12GraphicsCommandList* cmdList);
	void				Render_Alpha(ID3D12GraphicsCommandList* cmdList);
	void				Render_Instancing(ID3D12GraphicsCommandList* cmdList);
	void				Render_UI(ID3D12GraphicsCommandList* cmdList);

	
	//void Render_UI(ID3D12GraphicsCommandList* cmdList);
	

public:
	void Clear_Renderer();
	void Delete_Renderer(RenderType eType);

private:
	vector<CGameObject*>				m_vecObject[RENDER_END];

	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>	mPSOs;
	vector<ComPtr<ID3D12DescriptorHeap>>							mSrvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Device>							m_d3dDevice;
	UINT mCbvSrvDescriptorSize = 0;

public:
	virtual CComponent* Clone(void);
	static CRenderer* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize);

private:
	virtual void Free();

};