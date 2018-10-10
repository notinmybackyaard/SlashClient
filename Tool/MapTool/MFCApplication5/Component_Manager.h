#pragma once

#include "Base.h"
#include "Define.h"

class CComponent;

class CComponent_Manager
	: public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
public:
	enum COMTYPE {COM_MESH, COM_RENDERER, COM_TRANSFORM, COM_END};
	

private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager();

public:
	CComponent* Clone_Component(wchar_t* pTag, COMTYPE eType);
	CComponent* Clone_Component(wchar_t* pTag);
	HRESULT		Ready_Component(wchar_t* pTag, CComponent* pComponent, COMTYPE eType);
	unordered_map<wstring, CComponent*>			Get_ComponentMap(COMTYPE eType) { return m_mapComponent[eType]; }
private:
	CComponent* Find_Component(wchar_t* pTag, COMTYPE eType);


public:
	unordered_map<wstring, CComponent*>			m_mapComponent[COM_END];

private:
	virtual void Free();
};