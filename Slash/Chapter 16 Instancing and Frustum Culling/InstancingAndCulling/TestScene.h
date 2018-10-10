#pragma once

#include "Scene.h"

typedef struct uiInfo
{
	float fSize = 0.0f;
	float fMoveX = 0.0f;
	float fMoveY = 0.0f;
	float fScale = 0.0f;
}UIInfo;

class CTestScene
	: public CScene
{

private:
	explicit CTestScene(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize, bool isWarrior);
public:
	virtual ~CTestScene();

public:
	virtual HRESULT Initialize();
	virtual bool	Update(const GameTimer& gt);
	void UpdateOOBB();
	void UpdateUI();
	virtual void	Render(ID3D12GraphicsCommandList* cmdList);
	virtual void	CollisionProcess();
public:
	WORD         GetObjectCount() { return m_ObjectCount; }

private:
	HRESULT Load_Map();

public:
	static CTestScene* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, vector<ComPtr<ID3D12DescriptorHeap>> &srv, UINT srvSize, bool isWarrior);

	void UISetting();
private:
	virtual void Free();
private:
	vector<pair<const string, const string>> path;
	bool				m_IsWarrior = true;
private:
	WORD            m_ObjectCount = 0;
};