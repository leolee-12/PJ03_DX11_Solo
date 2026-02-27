#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

enum class EInputState : _byte
{
	Released = -1,
	Release,
	Pressing,
	Pressed
};

class ENGINE_DLL CInput_Device : public CBase
{
private:
	explicit CInput_Device(void);
	virtual ~CInput_Device(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	bool	Key_Pressing(_ubyte byKeyID);
	bool	Key_Down(_ubyte byKeyID);
	bool	Key_Up(_ubyte byKeyID);

	bool	Key_PressingEx(_ubyte byKeyID, DIK_EX byModifierType);
	bool	Key_DownEx(_ubyte byKeyID, DIK_EX byModifierType);
	bool	Key_UpEx(_ubyte byKeyID, DIK_EX byModifierType);

private:
	_bool	Modifier_Check(const DIK_EX& byModifierType);

public:
	bool	Mouse_Pressing(MOUSEKEYSTATE eMouse);
	bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	bool	Mouse_Up(MOUSEKEYSTATE eMouse);

	void	Mouse_Fix();

	_bool	Mouse_RayIntersect(SPACETYPE eSpacetype, _float3* pOut, _fvector vV1, _fvector vV2, _fvector vV3, _matrix matWorld, _float* pLengthOut);
	void	Mouse_ToWorld(_vector& vOrigin, _vector& vDir);

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void	Update_InputDev(void);

	void	LateUpdate_InputDev(void);
	_bool	Any_Input_Received();

private:
	_bool	CanUpdate();

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;
	HWND					m_hWnd;
	_uint					m_iWinSizeX = 0;
	_uint					m_iWinSizeY = 0;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState;

	EInputState				m_byKeyInputState[256];
	EInputState				m_tMouseInputState[DIM_END];

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
