#pragma once

#include "Macro.h"


class CInputDevice
{
	DECLARE_SINGLETON(CInputDevice)
public:
	enum INPUT		{ INPUT_NONE, INPUT_UP, INPUT_DOWN, INPUT_PRESS, };
	enum MOUSEINPUT { LEFT_BUTTON, RIGHT_BUTTON, WHEEL_BUTTON, EXTENSION_BUTTON, };
private:
	explicit CInputDevice(void);
	virtual ~CInputDevice(void);
public:
	HRESULT Ready_InputDevice(HWND hWnd, HINSTANCE hInst);
	void SetUp_InputDeviceState(void);
public:
	BYTE GetDIKeyState(short nKey) { return m_byKeyMap[nKey]; }
	bool AnyKeyInput(void);
public:
	float* GetMouseEpsX(void);
	float* GetMouseEpsY(void);

	XMFLOAT3* GetMousePos(void);
private:
	LPDIRECTINPUT8 m_pInput;
private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	BYTE m_byKState[256];
	BYTE m_byKeyCur[256];
	BYTE m_byKeyOld[256];
	BYTE m_byKeyMap[256];

	bool m_IsNone = false;
private:
	LPDIRECTINPUTDEVICE8 m_pMouse;
	DIMOUSESTATE m_MState;
	BYTE m_byMouseCur[4];
	BYTE m_byMouseOld[4];
	BYTE m_byMouseMap[4];
private:
	HWND m_hWnd;
	XMFLOAT3	m_vCur;
	XMFLOAT3	m_vOld;
	XMFLOAT3	m_vEps;
private:
	virtual void Release(void);
};
