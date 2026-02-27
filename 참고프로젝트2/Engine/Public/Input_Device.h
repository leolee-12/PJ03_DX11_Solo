#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte Get_DIMouseState(MOUSEKEYSTATE eMouse) { return m_tMouseState.rgbButtons[eMouse]; }
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState) { return *(((_long*)&m_tMouseState) + eMouseState); }
	_byte Get_DIMouseWheelState() { return m_tMouseState.lZ; }

	_bool Key_Pressing(_uint _iKey);
	_bool Key_Down(_uint _iKey);
	_bool Key_Up(_uint _iKey);

	_bool Mouse_Pressing(_uint _iButton);
	_bool Mouse_Down(_uint _iButton);
	_bool Mouse_Up(_uint _iButton);

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void   Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

	_byte m_byKeyState[256] = {}; // 키보드에 있는 모든 키값을 저장하기 위한 
	BYTE m_byPrevKeyState[256];   // Previous key state변수
	DIMOUSESTATE m_tMouseState = {};
	_bool m_bKeyState[256] = { false }; // 이전 키 상태를 저장하기 위한 변수
	_bool m_bMouseState[8] = { false }; // 이전 마우스 상태를 저장하기 위한 변수
	_bool m_bPrevMouseState[8] = { false }; // 이전 프레임의 마우스 상태를 저장하기 위한 변수

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void   Free(void);

};
END

