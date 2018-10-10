#pragma once

#include "GameObject.h"
#include "Renderer.h"
class Camera;
class CMesh;

constexpr      unsigned long MAXINSTOBJECTID = 100000;

class CInstancingObject
	: public CGameObject
{
	static bool m_bFinalCollisionCheck;
	typedef struct objdrawelement
	{
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	}DrawElement;

private:
	vector<bool> instancingCollisionState;
	int collision_i = 0;
public:
	explicit CInstancingObject(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* pMesh, int iSize);
	virtual ~CInstancingObject();
	virtual void		SaveSlidingVector(CGameObject * pobj, CGameObject * pCollobj);

public:
	void SetTexture(int matIdx, int idx) { mMaterials[m_iMyInstObject + matIdx]->DiffuseSrvHeapIndex = idx; }

	void SetRenderType(CRenderer::RenderType eType) { m_eMyRenderType = eType; }
public:
	virtual HRESULT			Initialize();
	virtual bool			Update(const GameTimer & gt);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);
	virtual void Animate(const GameTimer & gt); //애니메이션 상태 설정, 객체 이동, 회전 여기서 하면 됨
	void Animate(const GameTimer & gt, CTransform * transform);
public:
	std::vector<InstanceData> GetvecInstances() { return vecInstances; }
	CTransform* GetTransform(int idx) { if (idx > m_iSize) return nullptr; return m_vecTransCom[idx]; }
private:
	UINT						InstanceCount;
	std::vector<InstanceData>	vecInstances;


	//std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	static vector<std::unique_ptr<Material>>			mMaterials;

	MeshGeometry*				m_GeoBounds;

	int							m_iSize;

	vector<CTransform*>			m_vecTransCom;
	string						m_strMesh;


	unsigned long				m_iMyInstObject;

	CRenderer::RenderType		m_eMyRenderType;
	
public:

	static unsigned long		m_iAllInstObjectIndex;
	static CInstancingObject*	m_pAllInstObject[MAXINSTOBJECTID];

public:
	static CInstancingObject* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* pMesh, int iSize);


	DrawElement Element_Bounds;
	virtual void			RenderBounds(ID3D12GraphicsCommandList* cmdList);
private:
	virtual void			Free();
};