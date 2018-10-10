#pragma once

class CBase
{
protected:
	inline explicit CBase();
	inline virtual ~CBase();

protected:
	unsigned long			m_dwRefCnt = 0;			// 객체의 주소가 몇개의 포인터에서 공유되고 있는지 저장하는 멤버변수
	bool					m_IsEnable = true;		// 객체가 유효한지 확인하는 멤버변수

public:
	inline unsigned long AddRef();
	inline bool	IsEnable() { return m_IsEnable; }
	inline virtual void Set_Enable(bool isEnable) { m_IsEnable = isEnable; }

public:
	inline unsigned long Release();

protected:
	virtual void Free() = 0;
};

#include "Base.inl"