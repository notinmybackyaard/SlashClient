#pragma once

#include "Base.h"
#include "GameTimer.h"

class CGameObject;

class CLayer
	: public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer();

private:
	vector<CGameObject*>			m_vecObject;


public:
	inline CGameObject*				Get_Object(unsigned int iIdx) { if (iIdx >= m_vecObject.size()) return nullptr; return m_vecObject[iIdx]; }
	inline vector<CGameObject*>&	Get_ObjectList() { return m_vecObject; }

public:
	HRESULT			Initialize();
	bool			Update(const GameTimer& gt);
	void			Render(ID3D12GraphicsCommandList* cmdList);

public:
	HRESULT			Add_Object(CGameObject* pObj);

public:
	static CLayer*	Create();

private:
	virtual void	Free();
};