#pragma once

#include "Base.h"
#include "Define.h"

class CComponent;

class CComponent_Manager
	: public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager();

public:
	CComponent* Clone_Component(wchar_t* pTag);
	HRESULT		Ready_Component(wchar_t* pTag, CComponent* pComponent);

private:
	CComponent* Find_Component(wchar_t* pTag);


public:
	unordered_map<wstring, CComponent*>			m_mapComponent;

private:
	virtual void Free();
};