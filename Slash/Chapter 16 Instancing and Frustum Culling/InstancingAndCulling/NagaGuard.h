#pragma once

#include "GameObject.h"
#include "Protocol.h"

class Player;

class NagaGuard : public CGameObject
{
private:
	const float m_fScale = 0.08f;
private:
	NagaGuard(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize);
public:
	virtual ~NagaGuard();
public:
	virtual bool			Update(const GameTimer & gt);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);
	virtual HRESULT			Initialize();
	virtual void			Animate(const GameTimer & gt);
	virtual void			SaveSlidingVector(CGameObject * pobj, CGameObject * pCollobj);

	virtual void			Hit(float fDamage);

public:
	void SetTexture(SpiderType _tex);

private:
	Player * m_pPlayer = nullptr;
	string					m_strTexName[SPIDER_END];

public:
	static NagaGuard* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize);

};



class AnimateStateMachine_NagaGuard
	: public AnimateStateMachine
{
public:

public:
	explicit AnimateStateMachine_NagaGuard(CGameObject* pObj, wchar_t* pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END]);
	virtual ~AnimateStateMachine_NagaGuard();
private:
	HRESULT			Initialize();
	virtual void	AnimationStateUpdate(const GameTimer & gt);
	virtual void SetTimerTrueFalse();

private:
	CGameObject * m_pObject;
	wchar_t*		m_pMachineName;											// > 어떤 객체의 상태머신인지 이름을 저장하는 변수
	wchar_t*		m_pStateName[State::STATE_END];			// > 각 상태의 이름

	bool			m_IsSoundPlay[State::STATE_END];			// > 해당 애니메이션의 사운드가 실행됐는지 확인하는 변수, 애니메이션이 바뀌거나 끝났을 경우 false로 바뀜
	int				m_SoundFrame[State::STATE_END];			// > 해당 애니메이션의 몇번째 프레임에서 사운드가 나와야하는지 저장하고 있는 변수
	bool			m_IsEffectPlay[State::STATE_END];			// > 해당 애니메이션의 이펙트가 실행됐는지 확인하는 변수, 애니메이션이 바뀌거나 끝났을 경우 false로 바뀜
	int				m_EffectFrame[State::STATE_END];			// > 해당 애니메이션의 몇번째 프레임에서 이펙트가 나와야하는지 저장하고 있는 변수

public:
	static AnimateStateMachine_NagaGuard* Create(CGameObject* pObj, wchar_t* pMachineName, int SoundFrame[State::STATE_END], int EffectFrame[State::STATE_END]);

private:
	virtual void Free();
};