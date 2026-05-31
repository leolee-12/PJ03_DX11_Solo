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
	void Set_InputState(INPUT_STATE eState) { m_eInputState = eState; }
	INPUT_STATE Get_InputState() const { return m_eInputState; }

	// 절대 커서 좌표 - 매 Update에서 1회 캐시
	_float2 Get_CursorClientF()  const;	// 윈도우 클라이언트 좌표 (px)
	_bool   Is_Cursor_InClient() const { return m_bCursorInClient; }

	_byte Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte Get_DIMouseState(DIMB eMouse) { return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)]; }
	_long Get_DIMouseMove(DIMM eMouseState) { return *((reinterpret_cast<_long*>(&m_tMouseState)) + static_cast<_uint>(eMouseState)); }

	// 엣지 감지 + 필터 적용
	_bool Key_Down(_ubyte byKeyID);     // !prev && cur && 필터 통과
	_bool Key_Up(_ubyte byKeyID);       // prev && !cur && 필터 통과
	_bool Key_Pressing(_ubyte byKeyID); // cur && 필터 통과

	// 마우스 (필터 적용)
	_bool Mouse_Down(DIMB eMouse);
	_bool Mouse_Up(DIMB eMouse);
	_bool Mouse_Pressing(DIMB eMouse);
	_long Mouse_Move(DIMM eMouseState);

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void Update();

private:
	LPDIRECTINPUT8 m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pMouse = { nullptr };

	_byte m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE m_tMouseState = {};

	// 엣지 감지
	_byte m_byPrevKeyState[256] = {};
	_byte m_byPrevMouseButtons[4] = {};

	// 상태 필터
	INPUT_STATE m_eInputState = INPUT_STATE::GAMEPLAY;
	_uint m_iKeyGroupMap[256] = {};  // 키그룹 매핑 테이블 : 각 DIK_* -> KeyGroup 비트

	// 커서 캐시
	HWND m_hWnd = { nullptr };
	POINT m_ptCursorClient = { 0, 0 };
	_bool m_bCursorInClient = { false };

private:
	void Ready_KeyGroupMap();

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);

private:
	virtual void Free() override;
};

NS_END

#endif // InputDev_h__