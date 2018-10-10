//***************************************************************************************
// InstancingAndCullingApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "stdafx.h"
#include "InstancingAndCullingApp.h"
#include "InstancingObject.h"
#include "InputDevice.h"
#include "Management.h"
#include "Component_Manager.h"
#include "Texture_Manager.h"
#include "Renderer.h"
#include "TestScene.h"
#include "EffectScene.h"
#include "SelectScene.h"
#include "Network.h"
#include "DynamicMesh.h"
#include "StaticMesh.h"
#include "GeometryMesh.h"
#include "DynamicMeshSingle.h"
#include "BillboardMesh.h"
#include "Sound.h"
#include "Skeleton.h"
#include "Effect.h"
#include "SkillEffect.h"
#include "Effect_Manager.h"
#include "UIMesh.h"
#include "ResourceUploadBatch.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"
#include "DescriptorHeap.h"
#include "NumUI_Inst.h"
#include "StartScene.h"
#include "Minidump.h"

const int gNumFrameResources = 3;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.


typedef struct character
{
	int iTimeValue = 0;
	vector<Vertex> vecVertex;
	vector<int> vecIndex;

	int iNumVertex = 0;
	int iNumIndex = 0;

	vector<XMFLOAT2> uv;
	vector<int>		uvIndex;

	int iNumTexCnt = 0;
	int iNumTexIndex = 0;

	vector<Vertex> realvecVertex;

}Character;

//typedef struct animInfo
//{
//	vector<Character>				mapAnimationModel;			// 애니메이션 프레임 마다의 정점들
//	int								iAnimationFrameSize;		// 한 애니메이션 전체 프레임
//}AnimInfo;


enum class RenderLayer : int
{
	Opaque = 0,
	InstancingOpaque,
	Debug,
	Sky,
	Count
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MiniDump::Begin();

	try
	{
		InstancingAndCullingApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	MiniDump::End();
}

InstancingAndCullingApp::InstancingAndCullingApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

InstancingAndCullingApp::~InstancingAndCullingApp()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool InstancingAndCullingApp::Initialize()
{

	mSrvDescriptorHeap.resize(2);

	if (!D3DApp::Initialize())
		return false;

	AllocConsole();
	freopen("CONOUT$", "wt", stdout);

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	mCamera.SetPosition(0.0f, 2.0f, -15.0f);

	LoadTextures();
	LoadMeshes();
	CComponent* pCom = nullptr;
	//pCom = GeometryMesh::Create(md3dDevice);
	//CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Geometry", pCom);

	pCom = CBillboardMesh::Create(md3dDevice);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Billboard", pCom);

	

	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();

	//BuildShapeGeometry();
	//BuildSkullGeometry();
	//BuildBarrelGeometry();


	CRenderer* pRenderer = CRenderer::Create(md3dDevice, mSrvDescriptorHeap);
	CSound* pSound = CSound::Create();
	pSound->SoundFileLoadFromPath(L"Assets/Sound");

	NumUI_Inst* pNumUI = NumUI_Inst::Create(md3dDevice, mSrvDescriptorHeap[HEAP_INSTANCING], mCbvSrvUavDescriptorSize);
	

	CInputDevice::GetInstance()->Ready_InputDevice(mhMainWnd, mhAppInst);
	CManagement::GetInstance()->Init_Management(pRenderer, pNumUI);
	CManagement::GetInstance()->Set_Sound(pSound);

	//CNetwork::GetInstance()->InitSock(mhMainWnd);

	BuildPSOs();
	BuildMaterials();
	LoadEffects();
	BuildRenderItems();
	BuildFrameResources();

	// > Font Test
	//D3D12_VIEWPORT viewport = { 0.0f, 0.0f,
	//	static_cast<float>(800.f), static_cast<float>(600.f),
	//	D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
	//m_spriteBatch->SetViewport(viewport);

	//m_fontPos.x = 800.f / 2.f;
	//m_fontPos.y = 600.f / 2.f;
	// > 

	CManagement::GetInstance()->GetRenderer()->SetPSOs(mPSOs);
	CManagement::GetInstance()->Get_CurScene()->Set_MainCam(&mCamera);
	CManagement::GetInstance()->Get_CurScene()->Set_CamFrustum(&mCamFrustum);
	CManagement::GetInstance()->GetCbvSrvDescriptorSize() = mCbvSrvUavDescriptorSize;
	CManagement::GetInstance()->GetSrvDescriptorHeap() = mSrvDescriptorHeap;


	mCamera.Set_Object(CManagement::GetInstance()->Get_CurScene()->Find_Object(L"Layer_Player", 0));

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}


void InstancingAndCullingApp::OnResize()
{
	D3DApp::OnResize();

	mCamera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	BoundingFrustum::CreateFromMatrix(mCamFrustum, mCamera.GetProj());
}

void InstancingAndCullingApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);

	CInputDevice::GetInstance()->SetUp_InputDeviceState();

	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials(gt);
	//UpdateMaterialBuffer(gt);
	//UpdateInstanceData(gt);

	/*	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);*/

	CManagement::GetInstance()->Update(gt, mCurrFrameResource);

	UpdateMainPassCB(gt);
}

void InstancingAndCullingApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["InstancingOpaque"].Get()));


	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap[1].Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
	// set as a root descriptor.
	auto matBuffer = mCurrFrameResource->MaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(1, matBuffer->GetGPUVirtualAddress());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	// Bind all the textures used in this scene.
	mCommandList->SetGraphicsRootDescriptorTable(3, mSrvDescriptorHeap[1]->GetGPUDescriptorHandleForHeapStart() );

	mCommandList->SetPipelineState(mPSOs["InstancingOpaque"].Get());

	DrawRenderItems(mCommandList.Get(), mOpaqueRitems);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void InstancingAndCullingApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void InstancingAndCullingApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void InstancingAndCullingApp::OnMouseMove(WPARAM btnState, int x, int y)
{

	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	}

	if (mCamera.bFirstPersonView) //1인칭 모드에서
	{

		float dx = XMConvertToRadians(0.35f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.35f*static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void InstancingAndCullingApp::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		mCamera.Walk(20.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera.Walk(-20.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera.Strafe(-20.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera.Strafe(20.0f*dt);

	if (GetAsyncKeyState('1') & 0x8000)
		mFrustumCullingEnabled = true;

	if (GetAsyncKeyState('2') & 0x8000)
		mFrustumCullingEnabled = false;




	//mCamera.UpdateViewMatrix();
	mCamera.Update();
}

void InstancingAndCullingApp::AnimateMaterials(const GameTimer& gt)
{

}

void InstancingAndCullingApp::UpdateMainPassCB(const GameTimer& gt)
{
	XMMATRIX view = mCamera.GetView();
	XMMATRIX proj = mCamera.GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mCamera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void InstancingAndCullingApp::LoadTextures()
{
	// > Instancing Heap 2D Texture Load
	auto bricksTex = new Texture;
	bricksTex->Name = "bricksTex";
	bricksTex->Filename = L"Assets/Textures/bricks.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), bricksTex->Filename.c_str(),
		bricksTex->Resource, bricksTex->UploadHeap));

	// > Texture Name, Texture, Texture Type(Instacing Texture_2D / Default Texture_2D / Default Texture_Cube
	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(bricksTex->Name, bricksTex, CTexture_Manager::TEX_INST_2D)))
		MSG_BOX(L"brickTex Ready Failed");


	auto stoneTex = new Texture;
	stoneTex->Name = "stoneTex";
	stoneTex->Filename = L"Assets/Textures/stone.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), stoneTex->Filename.c_str(),
		stoneTex->Resource, stoneTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(stoneTex->Name, stoneTex, CTexture_Manager::TEX_INST_2D)))
		MSG_BOX(L"stoneTex Ready Failed");
	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(stoneTex->Name, stoneTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"InsecTex Ready Failed");

	auto tileTex = new Texture;
	tileTex->Name = "tileTex";
	tileTex->Filename = L"Assets/Textures/ice.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), tileTex->Filename.c_str(),
		tileTex->Resource, tileTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(tileTex->Name, tileTex, CTexture_Manager::TEX_INST_2D)))
		MSG_BOX(L"tileTex Ready Failed");

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(tileTex->Name, tileTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"tileTex Ready Failed");

	auto crateTex = new Texture;
	crateTex->Name = "crateTex";
	crateTex->Filename = L"Assets/Textures/WoodCrate01.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), crateTex->Filename.c_str(),
		crateTex->Resource, crateTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(crateTex->Name, crateTex, CTexture_Manager::TEX_INST_2D)))
		MSG_BOX(L"crateTex Ready Failed");

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(crateTex->Name, crateTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"crateTex Ready Failed");

	auto iceTex = new Texture;
	iceTex->Name = "iceTex";
	iceTex->Filename = L"Assets/Textures/ice.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), iceTex->Filename.c_str(),
		iceTex->Resource, iceTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(iceTex->Name, iceTex, CTexture_Manager::TEX_INST_2D)))		// 이거 텍스쳐 매니저 만들어서 애들 관리하고
		MSG_BOX(L"iceTex Ready Failed");

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(iceTex->Name, iceTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"iceTex Ready Failed");

	auto grassTex = new Texture;
	grassTex->Name = "grassTex";
	grassTex->Filename = L"Assets/Textures/grass.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), grassTex->Filename.c_str(),
		grassTex->Resource, grassTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(grassTex->Name, grassTex, CTexture_Manager::TEX_INST_2D)))
		MSG_BOX(L"grassTex Ready Failed");


	// > For NumUI Texture(Inst)
	auto NumUiTex = new Texture;
	NumUiTex->Name = "NumTex";
	NumUiTex->Filename = L"Assets/Textures/Num.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), NumUiTex->Filename.c_str(),
		NumUiTex->Resource, NumUiTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(NumUiTex->Name, NumUiTex, CTexture_Manager::TEX_INST_2D)))
		MSG_BOX(L"NumUiTex Ready Failed");

	// > Default Heap 2D Texture Load

	auto SpiderTex = new Texture;
	SpiderTex->Name = "SpiderTex";
	SpiderTex->Filename = L"Assets/Textures/spider.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), SpiderTex->Filename.c_str(),
		SpiderTex->Resource, SpiderTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(SpiderTex->Name, SpiderTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SpiderTex Ready Failed");

	/*auto defaultTex = std::make_unique<Texture>();
	defaultTex->Name = "defaultTex";
	defaultTex->Filename = L"Assets/Textures/white1x1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), defaultTex->Filename.c_str(),
		defaultTex->Resource, defaultTex->UploadHeap));*/

	auto InsecTex = new Texture;
	InsecTex->Name = "VillagerTex";
	InsecTex->Filename = L"Assets/Textures/villager02_diff.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), InsecTex->Filename.c_str(),
		InsecTex->Resource, InsecTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(InsecTex->Name, InsecTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"InsecTex Ready Failed");

	auto WarriorUltimateTex = new Texture;
	WarriorUltimateTex->Name = "WarriorUltimateTex";
	WarriorUltimateTex->Filename = L"Assets/Textures/WarriorUltimateTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), WarriorUltimateTex->Filename.c_str(),
		WarriorUltimateTex->Resource, WarriorUltimateTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(WarriorUltimateTex->Name, WarriorUltimateTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"WarriorUltimateTex Ready Failed");

	//

	auto NagaGuardTex = new Texture;
	NagaGuardTex->Name = "NagaGuardTex";
	NagaGuardTex->Filename = L"Assets/Textures/NagaGuardTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), NagaGuardTex->Filename.c_str(),
		NagaGuardTex->Resource, NagaGuardTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(NagaGuardTex->Name, NagaGuardTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"NagaGuardTex Ready Failed");

	auto RockWarriorTex = new Texture;
	RockWarriorTex->Name = "RockWarriorTex";
	RockWarriorTex->Filename = L"Assets/Textures/RockWarriorTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), RockWarriorTex->Filename.c_str(),
		RockWarriorTex->Resource, RockWarriorTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(RockWarriorTex->Name, RockWarriorTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"RockWarriorTex Ready Failed");

	auto MushroomTex = new Texture;
	MushroomTex->Name = "MushroomTex";
	MushroomTex->Filename = L"Assets/Textures/MushroomTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), MushroomTex->Filename.c_str(),
		MushroomTex->Resource, MushroomTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(MushroomTex->Name, MushroomTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"MushroomTex Ready Failed");

	auto TreeGuardTex = new Texture;
	TreeGuardTex->Name = "TreeGuardTex";
	TreeGuardTex->Filename = L"Assets/Textures/TreeGuardTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), TreeGuardTex->Filename.c_str(),
		TreeGuardTex->Resource, TreeGuardTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(TreeGuardTex->Name, TreeGuardTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"TreeGuardTex Ready Failed");

	//
	auto SkyTex = new Texture;
	SkyTex->Name = "SkyTex"; 
	SkyTex->Filename = L"Assets/Textures/desertcube1024.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
	mCommandList.Get(), SkyTex->Filename.c_str(),
		SkyTex->Resource, SkyTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(SkyTex->Name, SkyTex, CTexture_Manager::TEX_DEFAULT_CUBE)))
		MSG_BOX(L"SkyTex Ready Failed");

	auto FenceTex = new Texture;
	FenceTex->Name = "FenceTex";
	FenceTex->Filename = L"Assets/Textures/WireFence.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), FenceTex->Filename.c_str(),
		FenceTex->Resource, FenceTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(FenceTex->Name, FenceTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"FenceTex Ready Failed");

	auto DragonTex = new Texture;
	DragonTex->Name = "DragonTex";
	DragonTex->Filename = L"Assets/Textures/DragonTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), DragonTex->Filename.c_str(),
		DragonTex->Resource, DragonTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(DragonTex->Name, DragonTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"DragonTex Ready Failed");

	auto MageTex = new Texture;
	MageTex->Name = "MageTex";
	MageTex->Filename = L"Assets/Textures/MageTex.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), MageTex->Filename.c_str(),
		MageTex->Resource, MageTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(MageTex->Name, MageTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"MageTex Ready Failed");

	auto BloodTex = new Texture;
	BloodTex->Name = "BloodTex";
	BloodTex->Filename = L"Assets/Textures/blood.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), BloodTex->Filename.c_str(),
		BloodTex->Resource, BloodTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(BloodTex->Name, BloodTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"BloodTex Ready Failed");


	auto HeartTex = new Texture;
	HeartTex->Name = "HeartTex";
	HeartTex->Filename = L"Assets/Textures/PlayerStateUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), HeartTex->Filename.c_str(),
		HeartTex->Resource, HeartTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(HeartTex->Name, HeartTex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"HeartTex Ready Failed");

	auto WarriorUITex = new Texture;
	WarriorUITex->Name = "WarriorUITex";
	WarriorUITex->Filename = L"Assets/Textures/warriorUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), WarriorUITex->Filename.c_str(),
		WarriorUITex->Resource, WarriorUITex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(WarriorUITex->Name, WarriorUITex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"WarriorUITex Ready Failed");

	auto MageUITex = new Texture;
	MageUITex->Name = "MageUITex";
	MageUITex->Filename = L"Assets/Textures/MageUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), MageUITex->Filename.c_str(),
		MageUITex->Resource, MageUITex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(MageUITex->Name, MageUITex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"MageUITex Ready Failed"); 

	auto Tex = new Texture;
	Tex->Name = "SelectBackground";
	Tex->Filename = L"Assets/Textures/SelectBackground.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SelectBackground Ready Failed");

	Tex = new Texture;
	Tex->Name = "SelectWarrior";
	Tex->Filename = L"Assets/Textures/SelectWarrior.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SelectWarrior Ready Failed");


	Tex = new Texture;
	Tex->Name = "SelectWizard";
	Tex->Filename = L"Assets/Textures/SelectWizard.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SelectWizard Ready Failed");

	Tex = new Texture;
	Tex->Name = "PressEnter";
	Tex->Filename = L"Assets/Textures/PressEnter.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"PressEnter Ready Failed");

	Tex = new Texture;
	Tex->Name = "SelectCharacter";
	Tex->Filename = L"Assets/Textures/SelectCharacter.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SelectCharacter Ready Failed");

	Tex = new Texture;
	Tex->Name = "BarUI";
	Tex->Filename = L"Assets/Textures/BarUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"BarUI Ready Failed");
	Tex = new Texture;
	Tex->Name = "GageUI";
	Tex->Filename = L"Assets/Textures/GageUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"GageUI Ready Failed");

	Tex = new Texture;
	Tex->Name = "ExpUI";
	Tex->Filename = L"Assets/Textures/ExpUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"ExpUI Ready Failed");
	Tex = new Texture;
	Tex->Name = "HPUI";
	Tex->Filename = L"Assets/Textures/HPUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"HPUI Ready Failed");



	Tex = new Texture;
	Tex->Name = "HPUIBase";
	Tex->Filename = L"Assets/Textures/HPUIBase.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"HPUIBase Ready Failed");

	Tex = new Texture;
	Tex->Name = "GageUIBase";
	Tex->Filename = L"Assets/Textures/GageUIBase.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"GageUIBase Ready Failed");

	Tex = new Texture;
	Tex->Name = "ExpUIBase";
	Tex->Filename = L"Assets/Textures/ExpUIBase.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"ExpUIBase Ready Failed");

	Tex = new Texture;
	Tex->Name = "LevelUpUI";
	Tex->Filename = L"Assets/Textures/LevelUpUI.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"LevelUpUI Ready Failed");

	Tex = new Texture;
	Tex->Name = "PlayerLevelUIBack";
	Tex->Filename = L"Assets/Textures/PlayerLevelUIBack.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"PlayerLevelUIBack Ready Failed");

	Tex = new Texture;
	Tex->Name = "PlayerLevelUIWizard";
	Tex->Filename = L"Assets/Textures/PlayerLevelUIWizard.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"PlayerLevelUIWizard Ready Failed");

	Tex = new Texture;
	Tex->Name = "PlayerLevelUIWarrior";
	Tex->Filename = L"Assets/Textures/PlayerLevelUIWarrior.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"PlayerLevelUIWarrior Ready Failed");
	
	Tex = new Texture;
	Tex->Name = "Num_LV";
	Tex->Filename = L"Assets/Textures/Num_LV.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Num_LV Ready Failed");
	

	Tex = new Texture;
	Tex->Name = "SkillUIWizard";
	Tex->Filename = L"Assets/Textures/SkillUIWizard.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SkillUIWizard Ready Failed");

	Tex = new Texture;
	Tex->Name = "SkillUIWarrior";
	Tex->Filename = L"Assets/Textures/SkillUIWarrior.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SkillUIWarrior Ready Failed");

	Tex = new Texture;
	Tex->Name = "Warrior_1";
	Tex->Filename = L"Assets/Textures/Warrior_1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Warrior_1 Ready Failed");

	Tex = new Texture;
	Tex->Name = "Warrior_2";
	Tex->Filename = L"Assets/Textures/Warrior_2.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Warrior_2 Ready Failed");


	Tex = new Texture;
	Tex->Name = "Warrior_3";
	Tex->Filename = L"Assets/Textures/Warrior_3.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Warrior_3 Ready Failed");


	Tex = new Texture;
	Tex->Name = "Warrior_R";
	Tex->Filename = L"Assets/Textures/Warrior_R.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Warrior_R Ready Failed");


	Tex = new Texture;
	Tex->Name = "Warrior_Shift";
	Tex->Filename = L"Assets/Textures/Warrior_Shift.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Warrior_Shift Ready Failed");


	Tex = new Texture;
	Tex->Name = "Wizard_1";
	Tex->Filename = L"Assets/Textures/Wizard_1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Wizard_1 Ready Failed");


	Tex = new Texture;
	Tex->Name = "Wizard_2";
	Tex->Filename = L"Assets/Textures/Wizard_2.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Wizard_2 Ready Failed");


	Tex = new Texture;
	Tex->Name = "Wizard_3";
	Tex->Filename = L"Assets/Textures/Wizard_3.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Wizard_3 Ready Failed");


	Tex = new Texture;
	Tex->Name = "Wizard_R";
	Tex->Filename = L"Assets/Textures/Wizard_R.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Wizard_R Ready Failed");

	Tex = new Texture;
	Tex->Name = "Wizard_Shift";
	Tex->Filename = L"Assets/Textures/Wizard_Shift.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Wizard_Shift Ready Failed");

	Tex = new Texture;
	Tex->Name = "SkillNot";
	Tex->Filename = L"Assets/Textures/SkillNot.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"SkillNot Ready Failed");

	Tex = new Texture;
	Tex->Name = "StartScene";
	Tex->Filename = L"Assets/Textures/StartScene.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"StartScene Ready Failed");

	Tex = new Texture;
	Tex->Name = "Slash";
	Tex->Filename = L"Assets/Textures/Slash.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Slash Ready Failed");

	Tex = new Texture;
	Tex->Name = "TheEnd";
	Tex->Filename = L"Assets/Textures/TheEnd.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"TheEnd Ready Failed");

	Tex = new Texture;
	Tex->Name = "Aura0";
	Tex->Filename = L"Assets/Textures/Aura0.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Tex->Filename.c_str(),
		Tex->Resource, Tex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
		MSG_BOX(L"Aura0 Ready Failed");

	auto treeArrayTex = new Texture;
	treeArrayTex->Name = "treeArrayTex";
	treeArrayTex->Filename = L"Assets/Textures/treeArray2.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), treeArrayTex->Filename.c_str(),
		treeArrayTex->Resource, treeArrayTex->UploadHeap));

	if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(treeArrayTex->Name, treeArrayTex, CTexture_Manager::TEX_DEFAULT_BILLBOARD)))
		MSG_BOX(L"treeArrayTex Ready Failed");

	ifstream texIn("Assets/Data/EffectTexture.txt");
	int iSize = 0;
	texIn >> iSize;
	for (int i = 0; i < iSize; ++i)
	{
		string ignore;
		texIn >> ignore;
		
		auto Tex = new Texture;
		Tex->Name = ignore;
		texIn >> ignore;
		wstring fileName;
		fileName.assign(ignore.begin(), ignore.end());
		Tex->Filename = fileName;
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
			mCommandList.Get(), Tex->Filename.c_str(),
			Tex->Resource, Tex->UploadHeap));

		if (FAILED(CTexture_Manager::GetInstance()->Ready_Texture(Tex->Name, Tex, CTexture_Manager::TEX_DEFAULT_2D)))
			MSG_BOX(L"Effect Texture Ready Failed");
	}

	mMaterials_Instancing[bricksTex->Name] = std::move(bricksTex);
	mMaterials_Instancing[stoneTex->Name] = std::move(stoneTex);
	mMaterials_Instancing[tileTex->Name] = std::move(tileTex);
	mMaterials_Instancing[crateTex->Name] = std::move(crateTex);
	mMaterials_Instancing[iceTex->Name] = std::move(iceTex);
	mMaterials_Instancing[grassTex->Name] = std::move(grassTex);
	//mMaterials[defaultTex->Name] = std::move(defaultTex);
	mMaterials_Instancing[InsecTex->Name] = std::move(InsecTex);
	mMaterials_Instancing[SkyTex->Name] = std::move(SkyTex);
	mMaterials_Instancing[FenceTex->Name] = std::move(FenceTex);
	mMaterials_Instancing[SpiderTex->Name] = std::move(SpiderTex);
	mMaterials_Instancing[DragonTex->Name] = std::move(DragonTex);
	mMaterials_Instancing[MageTex->Name] = std::move(MageTex);
	mMaterials_Instancing[BloodTex->Name] = std::move(BloodTex);
	mMaterials_Instancing[HeartTex->Name] = std::move(HeartTex);
	mMaterials_Instancing[WarriorUITex->Name] = std::move(WarriorUITex);
	mMaterials_Instancing[MageUITex->Name] = std::move(MageUITex);
	

}

void InstancingAndCullingApp::LoadMeshes()
{

	vector<pair<const string, const string>> path;
	//path.push_back(make_pair("Attack", "Assets/Models/Warrior/Warrior_Attack_Turn.ASE"));
	//==
	//CComponent* pComponent;
	path.push_back(make_pair("Idle", "Assets/Models/Warrior/Warrior_Idle.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Warrior/Warrior_Walk.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Attack1.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Attack2.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Attack3.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Death.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Ultimate.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Roll.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Warrior/Warrior_Hit.ASE"));


	CComponent* pComponent = DynamicMesh::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Warrior", pComponent);
	//
	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/Mage/Mage_Idle.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Mage/Mage_Walk.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Attack1.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Spell1.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Spell2.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Death.ASE"));
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Ultimate.ASE")); // Ultimate
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Death.ASE")); // Roll
	path.push_back(make_pair("Back", "Assets/Models/Mage/Mage_Hit.ASE"));




	pComponent = DynamicMesh::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Mage", pComponent);
	
	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/Spider/Spider_Idle.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Spider/Spider_Walk.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Spider/Spider_Attack1.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Spider/Spider_Attack2.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Spider/Spider_Attack3.ASE"));
	path.push_back(make_pair("Walk", "Assets/Models/Spider/Spider_Dead.ASE"));

	CComponent* pComponentSingle = DynamicMeshSingle::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Spider", pComponentSingle);
	
	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/Dragon/Dragon_FlyIdle.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/Dragon/Dragon_FlyForward.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/Dragon/Dragon_FlyAttack.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/Dragon/Dragon_TakeOff.ASE"));
	pComponentSingle = DynamicMeshSingle::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Dragon", pComponentSingle);

	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/NagaGuard/NagaGuard_Idle.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/NagaGuard/NagaGuard_Walk.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/NagaGuard/NagaGuard_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/NagaGuard/NagaGuard_Attack2.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/NagaGuard/NagaGuard_Hit.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/NagaGuard/NagaGuard_Die.ASE"));

	pComponentSingle = DynamicMeshSingle::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_NagaGuard", pComponentSingle);

	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/RockWarrior/RockWarrior_Idle.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/RockWarrior/RockWarrior_Walk.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/RockWarrior/RockWarrior_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/RockWarrior/RockWarrior_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/RockWarrior/RockWarrior_Hit.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/RockWarrior/RockWarrior_Die.ASE"));

	pComponentSingle = DynamicMeshSingle::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_RockWarrior", pComponentSingle);

	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/ForestTurtle/ForestTurtle_Idle.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/ForestTurtle/ForestTurtle_Walk.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/ForestTurtle/ForestTurtle_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/ForestTurtle/ForestTurtle_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/ForestTurtle/ForestTurtle_Die.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/ForestTurtle/ForestTurtle_Hit.ASE"));



	pComponentSingle = DynamicMeshSingle::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Mushroom", pComponentSingle);

	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/TreeGuard/TreeGuard_Idle.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/TreeGuard/TreeGuard_Walk.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/TreeGuard/TreeGuard_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/TreeGuard/TreeGuard_Attack1.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/TreeGuard/TreeGuard_Hit.ASE"));
	path.push_back(make_pair("Idle", "Assets/Models/TreeGuard/TreeGuard_Die.ASE"));

	pComponentSingle = DynamicMeshSingle::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_TreeGuard", pComponentSingle);

	/*
	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/StaticMesh/staticMesh.ASE"));
	pComponent = StaticMesh::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Barrel", pComponent);
	
	path.clear();
	path.push_back(make_pair("Idle", "Assets/Models/StaticMesh/House1.ASE"));
	pComponent = StaticMesh::Create(md3dDevice, path);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_House", pComponent);
	*/
	pComponent = GeometryMesh::Create(md3dDevice);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Geometry", pComponent);

	XMFLOAT2 move = XMFLOAT2(-0.3f, 7.3f);
	XMFLOAT2 scale = XMFLOAT2(1.2f, 0.125f);
	move.x = -0.885f;
	move.y = 0.785f;

	scale.x = 1.0f;
	scale.y = 1.0f;

	float size = 0.08f;
	move.x = 0.f;
	move.y = -1.4f;
	scale.x = 0.7f;
	scale.y = 0.4f;
	size = 0.8f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_PressEnter", pComponent);

	move.x = 0.f;
	move.y = 1.5f;
	scale.x = 0.9f;
	scale.y = 0.4f;
	size = 0.8f;

	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_SelectCharacter", pComponent);

	move.x = 0.f;
	move.y = 0.f;
	scale.x = 1.f;
	scale.y = 1.f;
	size = 0.8f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Seletor0", pComponent);

	move.x = -0.885f;
	move.y = 0.785f;

	scale.x = 1.0f;
	scale.y = 1.0f;

	size = 0.08f;

	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_WarriorUI", pComponent);

	move.x = -0.94f;
	move.y = 5.2f;

	scale.x = 0.65f;
	scale.y = 0.15f;

	size = 1.f;

	// HP
	move.x = 0.f;
	move.y = -14.8f;
	scale.x = 0.7f;
	scale.y = 0.037f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.01f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_PlayerHPState", pComponent);

	// Exp
	move.x = 0.f;
	move.y = -16.5f;
	scale.x = 0.7f;
	scale.y = 0.029f;

	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.01f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_PlayerExpState", pComponent);

	// Gage
	move.x = 0.f;
	move.y = -19.5f;
	scale.x = 0.7f;
	scale.y = 0.032f;

	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.01f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_PlayerGageState", pComponent);


	move.x = -0.86f;
	move.y = 2.7f;
	scale.x = 0.6f;
	scale.y = 0.3f;

	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.002f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_PlayerLevelState", pComponent);

	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.001f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_PlayerJobState", pComponent);

	move.x = -3.6f;
	move.y = 12.5f;
	scale.x = 0.06f;
	scale.y = 0.06f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Num1", pComponent);

	move.x -= 1.f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Num", pComponent);

	move.x = 0.f;
	move.y = -4.f;
	scale.x = 0.6f;
	scale.y = 0.2f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.001f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_SkillUI", pComponent);


	move.x = 0.f;
	move.y = 0.f;
	scale.x = 1.f;
	scale.y = 1.f;
	size = 1.f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size, 0.001f);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_Ending", pComponent);


	move.x = 0.f;
	move.y = 0.f;
	scale.x = 0.7f;
	scale.y = 0.4f;
	size = 0.8f;
	pComponent = UIMesh::Create(md3dDevice, move, scale, size);
	CComponent_Manager::GetInstance()->Ready_Component(L"Com_Mesh_TheEnd", pComponent);

	// > Map Object ASE Load
	fstream in("Assets/Data/ModelList.txt");
	if (in.is_open() == false)
	{
		MSG_BOX(L"ModelList.txt 읽기 실패함 망함ㅠㅠㅠㅠㅠㅠㅠ");
	}

	string meshName;
	string meshPath;
	while (!(in.eof()))
	{
		in >> meshName;
		in >> meshPath;
		path.clear();
		path.push_back(make_pair("Idle", meshPath));
		pComponent = StaticMesh::Create(md3dDevice, path);
		wstring wstr;
		wstr.assign(meshName.begin(), meshName.end());
		CComponent_Manager::GetInstance()->Ready_Component(const_cast<wchar_t*>(wstr.c_str()), pComponent);
	}

	// > Effect Object ASE Load
	ifstream Modelin("Assets/Data/EffectModelList.txt");
	int iSize = 0;
	Modelin >> iSize;
	for (int i = 0; i < iSize; ++i)
	{
		Modelin >> meshName;
		Modelin >> meshPath;
		path.clear();
		path.push_back(make_pair("Idle", meshPath));
		pComponent = StaticMesh::Create(md3dDevice, path);
		wstring wstr;
		wstr.assign(meshName.begin(), meshName.end());
		CComponent_Manager::GetInstance()->Ready_Component(const_cast<wchar_t*>(wstr.c_str()), pComponent);

	}
}

void InstancingAndCullingApp::LoadEffects()
{

	ifstream in("Assets/Data/Effect.txt");
	int iSize;
	in >> iSize;
	
	for (int i = 0; i < iSize; ++i)
	{
		EFFECT_INFO info;

		in >> info.isBillboard >> info.strName >> info.strMeshName >> info.strTexName;
		in >> info.S_Pos.x >> info.S_Pos.y >> info.S_Pos.z;
		in >> info.S_Size.x >> info.S_Size.y >> info.S_Size.z;
		in >> info.S_Rot.x >> info.S_Rot.y >> info.S_Rot.z;
		in >> info.S_Color.x >> info.S_Color.y >> info.S_Color.z >> info.S_Color.w;
		in >> info.E_Pos.x >> info.E_Pos.y >> info.E_Pos.z;
		in >> info.E_Size.x >> info.E_Size.y >> info.E_Size.z;
		in >> info.E_Rot.x >> info.E_Rot.y >> info.E_Rot.z;
		in >> info.E_Color.x >> info.E_Color.y >> info.E_Color.z >> info.E_Color.w;
		in >> info.StartTime >> info.LifeTime;


		CEffect* effect = CEffect::Create(md3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, info);

		bool isFrame = false;
		in >> isFrame;



		if (isFrame)
		{
			UV_FRAME_INFO frameInfo;
			in >> frameInfo.f2FrameSize.x >> frameInfo.f2FrameSize.y;
			in >> frameInfo.f2maxFrame.x >> frameInfo.f2maxFrame.y;
			in >> frameInfo.fSpeed;
			effect->SetIsFrame(true);
			effect->Get_FrameInfo() = frameInfo;
		}
		//effect->Get_EffectInfo() = info;

		CEffect_Manager::GetInstance()->Ready_Effect(info.strName, effect);
	}

	ifstream skillIn("Assets/Data/EffectSkill.txt");
	skillIn >> iSize;
	for (int i = 0; i < iSize; ++i)
	{
		
		string name;
		skillIn >> name;

		CSkillEffect* skillEffect = CSkillEffect::Create(md3dDevice, mSrvDescriptorHeap[HEAP_DEFAULT], mCbvSrvDescriptorSize, name);

		int listSize = 0;
		skillIn >> listSize;

		list<CEffect*> list;

		for (int j = 0; j < listSize; ++j)
		{
			string name;
			skillIn >> name;
			CEffect* effect = CEffect_Manager::GetInstance()->Find_Effect(name);
			if (effect)
				list.push_back(effect);
		}
		skillEffect->GetEffectList() = list;
		CEffect_Manager::GetInstance()->Ready_SkillEffect(name, skillEffect);
	}
}

void InstancingAndCullingApp::BuildRootSignature()
{

	// Instancing - Space 0

	// Default - Space 1


	CD3DX12_DESCRIPTOR_RANGE texTable; //인스턴싱 텍스쳐
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 64, 2 ,0 );

	CD3DX12_DESCRIPTOR_RANGE texTable0; //스카이박스
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 1);

	CD3DX12_DESCRIPTOR_RANGE texTable1; //디폴트 텍스쳐
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2, 1);

	CD3DX12_DESCRIPTOR_RANGE texTable2; //빌보드 텍스쳐
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, 1);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[9];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsShaderResourceView(0, 0);
	slotRootParameter[1].InitAsShaderResourceView(1, 0);
	slotRootParameter[2].InitAsConstantBufferView(0);
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	//////////////////////////////////////////////////////
	slotRootParameter[4].InitAsConstantBufferView(0,1); //상수버퍼
	slotRootParameter[5].InitAsShaderResourceView(0,1); //Default 재질 스트럭쳐 버퍼로
	//slotRootParameter[5].InitAsConstantBufferView(1, 1); //Default 재질 스트럭쳐 버퍼로

	//////////////////////////////////////////////////////
	slotRootParameter[6].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[7].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[8].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(9, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void InstancingAndCullingApp::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	ComPtr<ID3D12DescriptorHeap> Heap;

	auto mapDefault2D = CTexture_Manager::GetInstance()->Get_TextureMap(CTexture_Manager::TEX_DEFAULT_2D);
	auto mapInst2D = CTexture_Manager::GetInstance()->Get_TextureMap(CTexture_Manager::TEX_INST_2D);
	auto mapDefaultCube = CTexture_Manager::GetInstance()->Get_TextureMap(CTexture_Manager::TEX_DEFAULT_CUBE);
	auto mapDefaultBill = CTexture_Manager::GetInstance()->Get_TextureMap(CTexture_Manager::TEX_DEFAULT_BILLBOARD);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc2 = {}; //Default Texture
	srvHeapDesc2.NumDescriptors = UINT(mapDefault2D.size()) + UINT(mapDefaultCube.size()) + UINT(mapDefaultBill.size());
	srvHeapDesc2.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc2.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc2, IID_PPV_ARGS(&mSrvDescriptorHeap[HEAP_DEFAULT])));


	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {}; //Instancing Texture
	srvHeapDesc.NumDescriptors = (UINT)mapInst2D.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap[HEAP_INSTANCING])));


	// > Font Test
	//m_resourceDescriptors = std::make_unique<DescriptorHeap>(md3dDevice.Get(),
	//	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	//	D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	//	Descriptors::Count);

	//ResourceUploadBatch resourceUpload(md3dDevice.Get());

	//resourceUpload.Begin();

	//m_font = std::make_unique<SpriteFont>(md3dDevice.Get(), resourceUpload,
	//	L"myfile.spritefont",
	//	m_resourceDescriptors->GetCpuHandle(Descriptors::MyFont),
	//	m_resourceDescriptors->GetGpuHandle(Descriptors::MyFont));

	//RenderTargetState rtState(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);

	//SpriteBatchPipelineStateDescription pd(rtState);
	//m_spriteBatch = std::make_unique<SpriteBatch>(md3dDevice.Get(), resourceUpload, pd);

	//auto uploadResourcesFinished = resourceUpload.End(mCommandQueue.Get());

	//uploadResourcesFinished.wait();

	// > 


	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor_Instancing(mSrvDescriptorHeap[HEAP_INSTANCING]->GetCPUDescriptorHandleForHeapStart()); // 인스턴싱

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor_Default(mSrvDescriptorHeap[HEAP_DEFAULT]->GetCPUDescriptorHandleForHeapStart()); // 디폴트

	auto bricksTex = mMaterials_Instancing["bricksTex"]->Resource;
	auto stoneTex = mMaterials_Instancing["stoneTex"]->Resource;
	auto tileTex = mMaterials_Instancing["tileTex"]->Resource;
	auto crateTex = mMaterials_Instancing["crateTex"]->Resource;
	auto iceTex = mMaterials_Instancing["iceTex"]->Resource;
	auto grassTex = mMaterials_Instancing["grassTex"]->Resource;
	//auto defaultTex = mMaterials["defaultTex"]->Resource;
	//auto skyTex = mMaterials["skyCubeMap"]->Resource;
	auto InsecTex = mMaterials_Instancing["VillagerTex"]->Resource;
	auto SkyTex = mMaterials_Instancing["SkyTex"]->Resource;
	auto FenceTex = mMaterials_Instancing["FenceTex"]->Resource;
	auto SpiderTex = mMaterials_Instancing["SpiderTex"]->Resource;
	auto DragonTex = mMaterials_Instancing["DragonTex"]->Resource;
	auto MageTex = mMaterials_Instancing["MageTex"]->Resource;
	auto BloodTex = mMaterials_Instancing["BloodTex"]->Resource;
	auto HeartTex = mMaterials_Instancing["HeartTex"]->Resource;
	auto WarriorUITex = mMaterials_Instancing["WarriorUITex"]->Resource;
	auto MageUITex = mMaterials_Instancing["MageUITex"]->Resource;

	// Instancing 

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_Instancing = {};
	srvDesc_Instancing.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_Instancing.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_Instancing.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc_Instancing.Texture2D.MostDetailedMip = 0;

	auto iter = mapInst2D.begin();
	auto iter_end = mapInst2D.end();
	int idx = 0;

	srvDesc_Instancing.Texture2D.MipLevels = iter->second->Resource->GetDesc().MipLevels;
	srvDesc_Instancing.Format = iter->second->Resource->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(iter->second->Resource.Get(), &srvDesc_Instancing, hDescriptor_Instancing);
	iter->second->Num = idx++;
	++iter;

	for (iter; iter != iter_end; ++iter)
	{
		hDescriptor_Instancing.Offset(1, mCbvSrvDescriptorSize);

		srvDesc_Instancing.Texture2D.MipLevels = iter->second->Resource->GetDesc().MipLevels;
		srvDesc_Instancing.Format = iter->second->Resource->GetDesc().Format;
		md3dDevice->CreateShaderResourceView(iter->second->Resource.Get(), &srvDesc_Instancing, hDescriptor_Instancing);
		iter->second->Num = idx++;
	}


	//Default Player

	// > For. Default 2D Texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_Default = {};

	iter = mapDefault2D.begin();
	iter_end = mapDefault2D.end();
	idx = 0;

	//auto desc = treeArrayTex->GetDesc();


	srvDesc_Default.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_Default.Texture2D.ResourceMinLODClamp = 0.0f;


	srvDesc_Default.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_Default.Texture2D.MostDetailedMip = 0;
	srvDesc_Default.Texture2D.MipLevels = iter->second->Resource->GetDesc().MipLevels;
	srvDesc_Default.Format = iter->second->Resource->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(iter->second->Resource.Get(), &srvDesc_Default, hDescriptor_Default);
	iter->second->Num = idx++;
	++iter;

	for (iter; iter != iter_end; ++iter)
	{
		hDescriptor_Default.Offset(1, mCbvSrvDescriptorSize);

		srvDesc_Default.Texture2D.MipLevels = iter->second->Resource->GetDesc().MipLevels;
		srvDesc_Default.Format = iter->second->Resource->GetDesc().Format;
		md3dDevice->CreateShaderResourceView(iter->second->Resource.Get(), &srvDesc_Default, hDescriptor_Default);
		iter->second->Num = idx++;
	}


	// next descriptor 10 SkyBox

	// > For. Default Cube Texture

	srvDesc_Default.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc_Default.TextureCube.MostDetailedMip = 0;
	srvDesc_Default.TextureCube.ResourceMinLODClamp = 0.0f;

	iter = mapDefaultCube.begin();
	iter_end = mapDefaultCube.end();
	//idx = 0;

	for (iter; iter != iter_end; ++iter)
	{
		hDescriptor_Default.Offset(1, mCbvSrvDescriptorSize);

		srvDesc_Default.TextureCube.MipLevels = iter->second->Resource->GetDesc().MipLevels;
		srvDesc_Default.Format = iter->second->Resource->GetDesc().Format;
		md3dDevice->CreateShaderResourceView(iter->second->Resource.Get(), &srvDesc_Default, hDescriptor_Default);
		iter->second->Num = idx++;

	}

	//srvDesc_Default.TextureCube.MipLevels = SkyTex->GetDesc().MipLevels;
	//srvDesc_Default.Format = SkyTex->GetDesc().Format;
	//md3dDevice->CreateShaderResourceView(SkyTex.Get(), &srvDesc_Default, hDescriptor_Default);


	// > For. Billboard Texture
	//iter = mapDefaultBill.begin();
	//iter_end = mapDefaultBill.end();
	//for (iter; iter != iter_end; ++iter)
	//{
	//	hDescriptor_Default.Offset(1, mCbvSrvDescriptorSize);

	//	srvDesc_Default.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	//	srvDesc_Default.Format = iter->second->Resource->GetDesc().Format;
	//	srvDesc_Default.Texture2DArray.MostDetailedMip = 0;
	//	srvDesc_Default.Texture2DArray.MipLevels = -1;
	//	srvDesc_Default.Texture2DArray.FirstArraySlice = 0;
	//	srvDesc_Default.Texture2DArray.ArraySize = iter->second->Resource->GetDesc().DepthOrArraySize;
	//	md3dDevice->CreateShaderResourceView(iter->second->Resource.Get(), &srvDesc_Default, hDescriptor_Default);
	//	iter->second->Num = idx++;
	//}
}

void InstancingAndCullingApp::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	mShaders["InstancingVS"] = d3dUtil::CompileShader(L"Shaders\\Instancing.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["InstancingPS"] = d3dUtil::CompileShader(L"Shaders\\Instancing.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["InstancingUI_VS"] = d3dUtil::CompileShader(L"Shaders\\Instancing.hlsl", nullptr, "VS_UI", "vs_5_1");
	mShaders["InstancingUI_PS"] = d3dUtil::CompileShader(L"Shaders\\Instancing.hlsl", alphaTestDefines, "PS_UI", "ps_5_1");

	mShaders["ObjectVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["ObjectPS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["skyVS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["skyPS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["UIVS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", alphaTestDefines, "VS", "vs_5_1");
	mShaders["UIPS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mShaders["UIChangePS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", alphaTestDefines, "PS_Change", "ps_5_1");

	mShaders["AlphaTestPS_Inst"] = d3dUtil::CompileShader(L"Shaders\\Instancing.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mShaders["treeSpriteVS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["treeSpriteGS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", nullptr, "GS", "gs_5_1");
	mShaders["treeSpritePS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mShaders["AlphaBelndVS"] = d3dUtil::CompileShader(L"Shaders\\Effect.hlsl", alphaTestDefines, "VS", "vs_5_1");
	mShaders["AlphaBelndPS"] = d3dUtil::CompileShader(L"Shaders\\Effect.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mShaders["AlphaBelndPS_Sprite"] = d3dUtil::CompileShader(L"Shaders\\Effect.hlsl", alphaTestDefines, "PS_SPRITE", "ps_5_1");
	
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	mBillboardLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}



void InstancingAndCullingApp::BuildPSOs()
{
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opaquePsoDesc.pRootSignature = mRootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["ObjectVS"]->GetBufferPointer()),
		mShaders["ObjectVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["ObjectPS"]->GetBufferPointer()),
		mShaders["ObjectPS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["Opaque"])));

	//
	// PSO for opaque objects.
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC InstancingOpaquePsoDesc = opaquePsoDesc;
	InstancingOpaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["InstancingVS"]->GetBufferPointer()),
		mShaders["InstancingVS"]->GetBufferSize()
	};
	InstancingOpaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["InstancingPS"]->GetBufferPointer()),
		mShaders["InstancingPS"]->GetBufferSize()
	};

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&InstancingOpaquePsoDesc, IID_PPV_ARGS(&mPSOs["InstancingOpaque"])));





	//
	// PSO for sky.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;

	// The camera is inside the sky sphere, so just turn off culling.
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //카메라가 구 내부에 있음으로 후면선별을 끔


																// Make sure the depth function is LESS_EQUAL and not just LESS.  
																// Otherwise, the normalized depth values at z = 1 (NDC) will 
																// fail the depth test if the depth buffer was cleared to 1.
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //깊이판정 LESS_EQUAL해야 구가 깊이판정 통과함
	skyPsoDesc.pRootSignature = mRootSignature.Get();

	skyPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["skyVS"]->GetBufferPointer()),
		mShaders["skyVS"]->GetBufferSize()
	};
	skyPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["skyPS"]->GetBufferPointer()),
		mShaders["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&mPSOs["sky"])));

	//
	// PSO for transparent objects
	//

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;

	//D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	//transparencyBlendDesc.BlendEnable = true;
	//transparencyBlendDesc.LogicOpEnable = false;
	//transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	//transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	//transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	//transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	//ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])));


	//
	// PSO for alpha tested objects
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPsoDesc = opaquePsoDesc;
	alphaTestedPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["InstancingVS"]->GetBufferPointer()),
		mShaders["InstancingVS"]->GetBufferSize()
	};

	alphaTestedPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["AlphaTestPS_Inst"]->GetBufferPointer()),
		mShaders["AlphaTestPS_Inst"]->GetBufferSize()
	};
	alphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&alphaTestedPsoDesc, IID_PPV_ARGS(&mPSOs["alphaTested_Inst"])));




	//
	// PSO for Alpha Blending objects
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaBlendPsoDesc = opaquePsoDesc;
	alphaBlendPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["AlphaBelndVS"]->GetBufferPointer()),
		mShaders["AlphaBelndVS"]->GetBufferSize()
	};
	alphaBlendPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["AlphaBelndPS_Sprite"]->GetBufferPointer()),
		mShaders["AlphaBelndPS_Sprite"]->GetBufferSize()
	};
	
	alphaBlendPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	alphaBlendPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;


	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&alphaBlendPsoDesc, IID_PPV_ARGS(&mPSOs["alphaBelnd"])));

	// PSO for Effect
	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaBlend_EffectPsoDesc = opaquePsoDesc;
	alphaBlend_EffectPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["AlphaBelndPS"]->GetBufferPointer()),
		mShaders["AlphaBelndPS"]->GetBufferSize()
	};

	alphaBlend_EffectPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc_effect;

	transparencyBlendDesc_effect.BlendEnable = true;
	transparencyBlendDesc_effect.LogicOpEnable = false;
	transparencyBlendDesc_effect.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc_effect.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc_effect.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc_effect.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc_effect.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc_effect.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc_effect.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc_effect.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	alphaBlend_EffectPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc_effect;


	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&alphaBlend_EffectPsoDesc, IID_PPV_ARGS(&mPSOs["alphaBelnd_Object"])));


	//
	// PSO for UI objects.
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC UIPsoDesc = opaquePsoDesc;
	UIPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["UIVS"]->GetBufferPointer()),
		mShaders["UIVS"]->GetBufferSize()
	};
	UIPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["UIPS"]->GetBufferPointer()),
		mShaders["UIPS"]->GetBufferSize()
	};
	UIPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc_effect;
	UIPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&UIPsoDesc, IID_PPV_ARGS(&mPSOs["UI"])));


	// > Instancing UI(NumUI)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC InstancingUIPsoDesc = opaquePsoDesc;
	InstancingUIPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["InstancingUI_VS"]->GetBufferPointer()),
		mShaders["InstancingUI_VS"]->GetBufferSize()
	};
	InstancingUIPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["InstancingUI_PS"]->GetBufferPointer()),
		mShaders["InstancingUI_PS"]->GetBufferSize()
	};

	InstancingUIPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc_effect;

	//InstancingUIPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&InstancingUIPsoDesc, IID_PPV_ARGS(&mPSOs["InstancingUI"])));

	//
	// PSO for UIChange objects.
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC UIChangePsoDesc = UIPsoDesc;
	//skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	UIChangePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["UIChangePS"]->GetBufferPointer()),
		mShaders["UIChangePS"]->GetBufferSize()
	};

	UIChangePsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc_effect;
	UIChangePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&UIChangePsoDesc, IID_PPV_ARGS(&mPSOs["UIChange"])));

	//
	// PSO for tree sprites
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC BillboardPsoDesc = opaquePsoDesc;
	BillboardPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["treeSpriteVS"]->GetBufferPointer()),
		mShaders["treeSpriteVS"]->GetBufferSize()
	};
	BillboardPsoDesc.GS =
	{
		reinterpret_cast<BYTE*>(mShaders["treeSpriteGS"]->GetBufferPointer()),
		mShaders["treeSpriteGS"]->GetBufferSize()
	};
	BillboardPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["treeSpritePS"]->GetBufferPointer()),
		mShaders["treeSpritePS"]->GetBufferSize()
	};
	BillboardPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	BillboardPsoDesc.InputLayout = { mBillboardLayout.data(), (UINT)mBillboardLayout.size() };
	BillboardPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&BillboardPsoDesc, IID_PPV_ARGS(&mPSOs["treeSprites"])));

}

void InstancingAndCullingApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			1, MAXOBJECTID, MAXOBJECTID));
	}
}

void InstancingAndCullingApp::BuildMaterials()
{
	///////////////////////////////////////////////////////////////////// Instancing.hlsl
	auto bricks0 = std::make_unique<Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto stone0 = std::make_unique<Material>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 1;
	stone0->DiffuseSrvHeapIndex = 1;
	stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 2;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.3f;

	auto crate0 = std::make_unique<Material>();
	crate0->Name = "checkboard0";
	crate0->MatCBIndex = 3;
	crate0->DiffuseSrvHeapIndex = 3;
	crate0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	crate0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	crate0->Roughness = 0.2f;

	auto ice0 = std::make_unique<Material>();
	ice0->Name = "ice0";
	ice0->MatCBIndex = 4;
	ice0->DiffuseSrvHeapIndex = 4;
	ice0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ice0->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	ice0->Roughness = 0.0f;

	auto grass0 = std::make_unique<Material>();
	grass0->Name = "grass0";
	grass0->MatCBIndex = 5;
	grass0->DiffuseSrvHeapIndex = 5;
	grass0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	grass0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	grass0->Roughness = 0.2f;

	///////////////////////////////////////////////////////////////////// Default.hlsl
	auto skullMat = std::make_unique<Material>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 6;
	skullMat->DiffuseSrvHeapIndex = 6;
	skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.5f;


	///////////////////////////////////////////////////////////////////// Sky.hlsl
	auto sky = std::make_unique<Material>();
	sky->Name = "sky";
	sky->MatCBIndex = 7;
	sky->DiffuseSrvHeapIndex = 7;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;

	mMaterials["bricks0"] = std::move(bricks0);
	mMaterials["stone0"] = std::move(stone0);
	mMaterials["tile0"] = std::move(tile0);
	mMaterials["crate0"] = std::move(crate0);
	mMaterials["ice0"] = std::move(ice0);
	mMaterials["grass0"] = std::move(grass0);

	////////////////////////////////////////////////////////
	mMaterials["skullMat"] = std::move(skullMat);
	mMaterials["sky"] = std::move(sky);
}

void InstancingAndCullingApp::BuildRenderItems()
{

	//CScene* pScene = CTestScene::Create(md3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize);
	//CScene* pScene = CSelectScene::Create(md3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize);
	CScene* pScene = CStartScene::Create(md3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize);
	CManagement::GetInstance()->Change_Scene(pScene);
	CEffect_Manager::GetInstance()->Ready_EffectManager(md3dDevice, mSrvDescriptorHeap, mCbvSrvDescriptorSize);
}

void InstancingAndCullingApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems )
{


	/*ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap_InstancingTex.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);*/


	CManagement::GetInstance()->Render(cmdList);

	// > Font Test
	//ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap() };
	//cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

	//m_spriteBatch->Begin(cmdList);

	//const wchar_t* output = L"Hello World";

	//XMFLOAT2 origin;
	////XMFLOAT2 f2origin;
	//XMStoreFloat2(&origin, m_font->MeasureString(output) / 2.f);

	//XMVECTORF32 White = { 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	//m_font->DrawString(m_spriteBatch.get(), output,
	//	m_fontPos, White, 0.f, origin);

	//m_spriteBatch->End();
	// > 

	//mCommandList->SetPipelineState(mPSOs["Opaque"].Get());

	//m_vecObjects[0]->Render(cmdList); // 거미

	//m_vecObjects[1]->Render(cmdList); // 배럴

	//m_vecObjects[2]->Render(cmdList); // 지형

	//mCommandList->SetPipelineState(mPSOs["sky"].Get());
	//m_vecObjects[3]->Render(cmdList); // 스카이박스


	//ID3D12DescriptorHeap* descriptorHeaps2[] = { mSrvDescriptorHeap_InstancingTex.Get() };
	//mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps2), descriptorHeaps2);

	//mCommandList->SetPipelineState(mPSOs["InstancingOpaque"].Get());
	//m_vecObjects[4]->Render(cmdList);

}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> InstancingAndCullingApp::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}
