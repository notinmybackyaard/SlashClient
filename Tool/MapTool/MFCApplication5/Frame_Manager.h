#pragma once
#include "Base.h"
#include "Define.h"

class CFrame;
class GameTimer;

class CFrame_Manager
	: public CBase
{
	DECLARE_SINGLETON(CFrame_Manager)
private:
	explicit CFrame_Manager();
public:
	virtual ~CFrame_Manager();

public:
	HRESULT Ready_Frames(const wchar_t* pTag, const float& fLimitCall);
	bool Permit_Call(const wchar_t* pTag, const GameTimer& gt);

private:
	unordered_map<const wchar_t*, CFrame*>		m_mapFrames;

private:
	CFrame* Find_Frame(const wchar_t* pTag);
	virtual void Free();
};