#pragma once
#include "Base.h"
#include "Define.h"

class GameTimer;

class CFrame
	: public CBase
{
private:
	explicit CFrame();
public:
	virtual ~CFrame();

public:
	HRESULT Ready_Frame(const float& fCallCnt);
	bool	Permit_Call(const GameTimer& gt);

private:
	float			m_fCallPerSec;
	float			m_fTimeDeltaAcc;

public:
	static CFrame* Create(const float& fCallCnt);

private:
	virtual void Free();
};