 #pragma once

#include "GameObject.h"
#include "Define.h"

class CSkeleton
	: public CGameObject
{
private:
	explicit CSkeleton(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* meshName, bool isWarrior);
public:
	virtual ~CSkeleton();

public:
	virtual void OnPrepareRender();
	virtual void Animate(const GameTimer & gt);


private:

	vector<pair<const string, const string>>			m_vecMeshPath;
	MeshGeometry* Geo_Body = nullptr;
	MeshGeometry* Geo_Head = nullptr;
	MeshGeometry* Geo_Left = nullptr;
	MeshGeometry* Geo_Right = nullptr;

	DrawElement Element_Body;
	DrawElement Element_Head;
	DrawElement Element_Left;
	DrawElement Element_Right;

	bool		m_IsWarrior;


public:
	virtual HRESULT			Initialize();
	virtual bool			Update(const GameTimer& gt);
	virtual void			Render(ID3D12GraphicsCommandList * cmdList);

public:
	void					Render_Body(ID3D12GraphicsCommandList* cmdList);
	void					Render_Head(ID3D12GraphicsCommandList* cmdList);
	void					Render_Right(ID3D12GraphicsCommandList* cmdList);

	virtual void Rotate(float fPitch, float fYaw, float fRoll);

public:
	static CSkeleton* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize, wchar_t* meshName, bool isWarrior);

	void Move(const XMFLOAT3 & xmf3Shift, bool bVelocity);

public:
	void SetCurState(int state) {
		m_CurState = state;
	}
	void SetPreState(int state) {
		m_PreState = state;
	}
	int GetCurState() {
		return m_CurState;
	}
	int GetPreState() {
		return m_PreState;
	}
	bool GetIsWarrior() {
		return m_IsWarrior;
	}

	void SetUltimateEffect(bool isUltimate);

	bool bIsUltimateState = false;
private:
	int m_CurState;
	int m_PreState;
private:
	virtual void Free();

};


class AnimateStateMachine_Skeleton
	: public AnimateStateMachine
{
public:
	explicit AnimateStateMachine_Skeleton(CGameObject* pObj, wchar_t* pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END]);
	virtual ~AnimateStateMachine_Skeleton();
private:
	HRESULT			Initialize();
	virtual void	AnimationStateUpdate(const GameTimer & gt);

public:
	void			SetUltimateEffect(bool isUltimate);

private:
	CGameObject*	m_pObject;
	wchar_t*		m_pMachineName;											// > 어떤 객체의 상태머신인지 이름을 저장하는 변수
	wchar_t*		m_pStateName[State::STATE_END];			// > 각 상태의 이름

	bool			m_IsSoundPlay[State::STATE_END];			// > 해당 애니메이션의 사운드가 실행됐는지 확인하는 변수, 애니메이션이 바뀌거나 끝났을 경우 false로 바뀜
	int				m_SoundFrame[State::STATE_END];			// > 해당 애니메이션의 몇번째 프레임에서 사운드가 나와야하는지 저장하고 있는 변수
	bool			m_IsEffectPlay[State::STATE_END];			// > 해당 애니메이션의 이펙트가 실행됐는지 확인하는 변수, 애니메이션이 바뀌거나 끝났을 경우 false로 바뀜
	int				m_EffectFrame[State::STATE_END];			// > 해당 애니메이션의 몇번째 프레임에서 이펙트가 나와야하는지 저장하고 있는 변수

	unordered_map<State, string>	m_mapEffectName;
public:
	static AnimateStateMachine_Skeleton* Create(CGameObject* pObj, wchar_t* pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END]);

private:
	virtual void Free();
};