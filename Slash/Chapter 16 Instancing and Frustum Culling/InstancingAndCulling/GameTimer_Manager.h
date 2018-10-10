#pragma once
#include "Base.h"
#include "Define.h"


class GameTimer;

class CGameTimer_Manager
	: public CBase
{
	DECLARE_SINGLETON(CGameTimer_Manager)
private:
	explicit CGameTimer_Manager();
public:
	virtual ~CGameTimer_Manager();

public:
	GameTimer* Get_GameTimer(const wchar_t* pTag);

public:
	HRESULT Ready_Timers(const wchar_t* pTag);
	void Compute_TimeDelta(const wchar_t* pTag);

private:
	unordered_map<const wchar_t*, GameTimer*>			m_mapTimers;

private:
	GameTimer* Find_GameTimer(const wchar_t* pTag);

private:
	virtual void Free();
};