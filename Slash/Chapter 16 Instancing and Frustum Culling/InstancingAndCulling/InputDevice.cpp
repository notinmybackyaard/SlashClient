#include "stdafx.h"
#include "InputDevice.h"

IMPLEMENT_SINGLETON(CInputDevice);

CInputDevice::CInputDevice( void )
: m_pInput(NULL)
, m_pKeyBoard(NULL)
, m_pMouse(NULL)
{
	ZeroMemory(m_byKeyCur, sizeof(m_byKeyCur));
	ZeroMemory(m_byKeyOld, sizeof(m_byKeyOld));
	ZeroMemory(m_byKeyMap, sizeof(m_byKeyMap));

	ZeroMemory(m_byMouseCur, sizeof(m_byMouseCur));
	ZeroMemory(m_byMouseOld, sizeof(m_byMouseOld));
	ZeroMemory(m_byMouseMap, sizeof(m_byMouseMap));

	ZeroMemory(&m_MState, sizeof(DIMOUSESTATE));

	m_vCur = XMFLOAT3(0, 0, 0);
	m_vOld = XMFLOAT3(0, 0, 0);
	m_vEps = XMFLOAT3(0, 0, 0);
}

CInputDevice::~CInputDevice( void )
{
}

HRESULT CInputDevice::Ready_InputDevice(HWND hWnd, HINSTANCE hInst)
{
	if ( FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL)) )
		return E_FAIL;

	m_hWnd = hWnd;

	// 키보드
	m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL);
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoard->Acquire();

	// 마우스
	m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->Acquire();

	int i = ShowCursor(TRUE);

	return S_OK;
}

void CInputDevice::SetUp_InputDeviceState( void )
{
	m_pKeyBoard->GetDeviceState(sizeof(m_byKState), m_byKState);
	m_pMouse->GetDeviceState(sizeof(m_MState), (LPVOID)&m_MState);

	// 키보드 상태 체크
	memcpy(m_byKeyOld, m_byKeyCur, sizeof(m_byKeyOld));
	memset(m_byKeyCur, 0, sizeof(m_byKeyCur));
	memset(m_byKeyMap, 0, sizeof(m_byKeyMap));

	for (int i = 0; i < 256; ++i)
	{
		BYTE vKey = m_byKState[i] & 0x80;
		m_byKeyCur[i] = (vKey) ? 1 : 0;

		int nOld = m_byKeyOld[i];
		int nCur = m_byKeyCur[i];

		if (nOld == 0 && nCur == 1)
			m_byKeyMap[i] = INPUT_DOWN;
		else if (nOld == 1 && nCur == 0)
			m_byKeyMap[i] = INPUT_UP;
		else if (nOld == 1 && nCur == 1)
			m_byKeyMap[i] = INPUT_PRESS;
		else
			m_byKeyMap[i] = INPUT_NONE;
	}

	// 마우스 상태 체크
	memcpy(m_byMouseOld, m_byMouseCur, sizeof(m_byMouseOld));
	memset(m_byMouseCur, 0, sizeof(m_byMouseCur));
	memset(m_byMouseMap, 0, sizeof(m_byMouseMap));

	for (int i = 0; i < 4; ++i)
	{
		BYTE vKey = m_MState.rgbButtons[i] & 0x80;
		m_byMouseCur[i] = (vKey) ? 1 : 0;

		int nOld = m_byMouseOld[i];
		int nCur = m_byMouseCur[i];

		if (nOld == 0 && nCur == 1)
			m_byMouseMap[i] = INPUT_DOWN;
		else if (nOld == 1 && nCur == 0)
			m_byMouseMap[i] = INPUT_UP;
		else if (nOld == 1 && nCur == 1)
			m_byMouseMap[i] = INPUT_PRESS;
		else
			m_byMouseMap[i] = INPUT_NONE;
	}

	//SetCursorPos(400, 300);

	m_vEps.x = (float)m_MState.lX;
	m_vEps.y = (float)m_MState.lY;

	if ( m_MState.lZ > 0 )
	{
		m_vEps.z += 1.f;
		if ( m_vEps.z > 3.f )
			m_vEps.z = 3.f;
	}
	else if ( m_MState.lZ < 0 )
	{
		m_vEps.z -= 1.f;
		if ( m_vEps.z < -3.f )
			m_vEps.z = -3.f;
	}
}

bool CInputDevice::AnyKeyInput(void)
{
	for (int i = 0; i < 256; ++i)
	{
		if ( m_byKeyCur[i] != INPUT_NONE)
			return FALSE;
	}

	return TRUE;
}

float* CInputDevice::GetMouseEpsX( void )
{
	return &m_vEps.x;
}

float* CInputDevice::GetMouseEpsY( void )
{
	return &m_vEps.y;
}


XMFLOAT3* CInputDevice::GetMousePos( void )
{
	return &m_vCur;
}

void CInputDevice::Release( void )
{
	if (m_pInput)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}

	if (m_pKeyBoard)
	{
		m_pKeyBoard->Release();
		m_pKeyBoard = nullptr;
	}
	
	if (m_pMouse)
	{
		m_pMouse->Release();
		m_pMouse = nullptr;
	}
}