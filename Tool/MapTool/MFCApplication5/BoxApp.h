#pragma once

#include "d3dApp.h"

#include "Camera.h"

class CGameObject;



class InstancingAndCullingApp : public D3DApp
{

public:
	InstancingAndCullingApp(HINSTANCE hInstance);
	InstancingAndCullingApp(const InstancingAndCullingApp& rhs) = delete;
	InstancingAndCullingApp& operator=(const InstancingAndCullingApp& rhs) = delete;
	~InstancingAndCullingApp();

	virtual bool Initialize()override;
	//////////////////////////////////
private:
	virtual void OnResize()override;
public:
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;
private:
	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	//
	//

	void LoadMesh();
	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();

	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	////////////////////

private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	//ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap_InstancingTex = nullptr;
	//ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap_Default= nullptr;

	vector<ComPtr<ID3D12DescriptorHeap>> mSrvDescriptorHeap;			// 0. Default		1. InstancingTex


	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

	std::unordered_map<std::string, Texture*> mMaterials_Instancing;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mOpaqueRitems;

	bool mFrustumCullingEnabled = true;

	BoundingFrustum mCamFrustum;

	PassConstants mMainPassCB;

	Camera mCamera;

	POINT mLastMousePos;

	vector<CGameObject*>			m_vecObjects;
};
