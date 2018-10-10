#pragma once
#include "Component.h"
#include "GameTimer.h"
#include "Define.h"



class Mesh
	: public CComponent
{
public:
	enum MESHTYPE { MESH_STATIC, MESH_STATIC_INST, MESH_DYNAMIC, MESH_DYNAMIC_SING };
public:
	XMVECTOR vMin;

	XMVECTOR vMax;
public:
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

	typedef struct bone
	{

		int iTimeValue = 0;

		string boneName;

		vector<Vertex> vecVertex;
		vector<int> vecIndex;

		int iNumVertex = 0;
		int iNumIndex = 0;

		vector<XMFLOAT2> uv;
		vector<int>		uvIndex;

		int iNumTexCnt = 0;
		int iNumTexIndex = 0;

		vector<Vertex> realvecVertex;

		static bool Comp(const bone &t1, const bone &t2)
		{
			return (t1.boneName < t2.boneName);
		}


	}Bone;


	typedef struct animInfo
	{
		vector<Mesh::Character>			AnimationModel;			// 애니메이션 프레임 마다의 정점들
		int								iAnimationFrameSize;		// 한 애니메이션 전체 프레임
	}AnimInfo;


public:
	explicit Mesh(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);
	virtual ~Mesh();

public:
	string			Get_TexName() { return m_strTexName; }

public:
	//virtual HRESULT Initialize(const string& pFilePath);
	virtual int Update(const GameTimer& gt);
	virtual void Render();
	virtual void Free();

public:
	virtual CComponent* Clone() = 0;
public:
	//virtual HRESULT LoadMesh(const wchar_t* AnimName, const char* pFilePath) = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D12Device>		m_d3dDevice;
	string				m_strTexName;
};


