#pragma once
#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class CInput_Device final : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte Get_DIMouseState(DIMB eMouse) { return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)]; }
	_long Get_DIMouseMove(DIMM eMouseState) { return *((reinterpret_cast<_long*>(&m_tMouseState)) + static_cast<_uint>(eMouseState)); }

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void Update(void);

private:
	LPDIRECTINPUT8 m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pMouse = { nullptr };

	_byte m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE m_tMouseState = {};

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);

private:
	virtual void Free() override;
};

NS_END

#endif // InputDev_h__