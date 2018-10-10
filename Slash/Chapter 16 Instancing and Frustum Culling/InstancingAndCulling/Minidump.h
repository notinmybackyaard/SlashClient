#pragma once
#pragma once
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <DbgHelp.h>

class MiniDump
{
public:
	static BOOL Begin(VOID);
	static BOOL End(VOID);
};