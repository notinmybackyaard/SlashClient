#pragma once

#include "Base.h"

class GameTimer;

class CComponent
	: public CBase
{
protected:
	explicit CComponent();
	virtual ~CComponent();

public:
	virtual CComponent* Clone(void) = 0;
public:
	virtual void Update_Component(const GameTimer & gt);
protected:
	virtual void Free(void);

};