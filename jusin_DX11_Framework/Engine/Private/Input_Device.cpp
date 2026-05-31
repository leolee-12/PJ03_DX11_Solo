#include "Input_Device.h"

CInput_Device::CInput_Device(void)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
}

_float2 CInput_Device::Get_CursorClientF() const
{
	return _float2(	static_cast<_float>(m_ptCursorClient.x),
					static_cast<_float>(m_ptCursorClient.y));
}

HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(	hInst,
									DIRECTINPUT_VERSION,
									IID_IDirectInput8,
									(void**)&m_pInputSDK,
									NULL)))
									return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();



	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	Ready_KeyGroupMap();

	return S_OK;
}

_bool CInput_Device::Key_Down(_ubyte byKeyID)
{
	if (!(m_byPrevKeyState[byKeyID] & 0x80) && (m_byKeyState[byKeyID] & 0x80))
	{
		_uint iGroup = m_iKeyGroupMap[byKeyID];
		_uint iAllowed = KeyGroup::s_AllowedGroups[ETOUI(m_eInputState)];
		return (iGroup & iAllowed) != 0;
	}
	
	return false;
}

_bool CInput_Device::Key_Up(_ubyte byKeyID)
{
	if ((m_byPrevKeyState[byKeyID] & 0x80) && !(m_byKeyState[byKeyID] & 0x80))
	{
		_uint iGroup = m_iKeyGroupMap[byKeyID];
		_uint iAllowed = KeyGroup::s_AllowedGroups[ETOUI(m_eInputState)];
		return (iGroup & iAllowed) != 0;
	}
		return false;
}

_bool CInput_Device::Key_Pressing(_ubyte byKeyID)
{
	if (!(m_byKeyState[byKeyID] & 0x80))
		return false;

	_uint iGroup = KeyGroup::CAMERA;
	_uint iAllowed = KeyGroup::s_AllowedGroups[ETOUI(m_eInputState)];
	return (iGroup & iAllowed) != 0;
}

_bool CInput_Device::Mouse_Down(DIMB eMouse)
{
	_byte byMouseID = static_cast<_byte>(eMouse);

	if (!(m_byPrevMouseButtons[byMouseID] & 0x80) && (m_tMouseState.rgbButtons[byMouseID] & 0x80))
		return true;
	return false;
}

_bool CInput_Device::Mouse_Up(DIMB eMouse)
{
	_byte byMouseID = static_cast<_byte>(eMouse);

	if ((m_byPrevMouseButtons[byMouseID] & 0x80) && !(m_tMouseState.rgbButtons[byMouseID] & 0x80))
		return true;
	return false;
}

_bool CInput_Device::Mouse_Pressing(DIMB eMouse)
{
	_byte byMouseID = static_cast<_byte>(eMouse);

	if (!(m_tMouseState.rgbButtons[byMouseID] & 0x80))
		return false;
	return true;
}

_long CInput_Device::Mouse_Move(DIMM eMouseState)
{
	_uint iAllowed = KeyGroup::s_AllowedGroups[ETOUI(m_eInputState)];
	if (!(KeyGroup::CAMERA & iAllowed))
		return 0L;  // CAMERA 그룹 차단된 상태면 0 반환

	return Get_DIMouseMove(eMouseState);
}

void CInput_Device::Update()
{
	// 0. 절대 커서 좌표 캐시 (Key/Mouse 갱신 전)
	{
		POINT pt{};
		if (m_hWnd && GetCursorPos(&pt) && ScreenToClient(m_hWnd, &pt))
		{
			m_ptCursorClient = pt;

			RECT rcClient{};
			if (GetClientRect(m_hWnd, &rcClient))
			{
				m_bCursorInClient =
					(pt.x >= rcClient.left && pt.x < rcClient.right &&
						pt.y >= rcClient.top && pt.y < rcClient.bottom);
			}
			else
			{
				m_bCursorInClient = false;
			}
		}
		else
		{
			m_bCursorInClient = false;
		}
	}

	// 1. 이전 상태 저장
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(m_byKeyState));
	memcpy(m_byPrevMouseButtons, m_tMouseState.rgbButtons, sizeof(m_byPrevMouseButtons));

	// 2. 현재 상태 갱신
	HRESULT hr = m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	if(FAILED(hr))
		m_pKeyBoard->Acquire(); // 입력 장치가 포커스를 잃었을 때 다시 획득 시도

	hr = m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
	if(FAILED(hr))
		m_pMouse->Acquire(); // 입력 장치가 포커스를 잃었을 때 다시 획득 시도
}

void CInput_Device::Ready_KeyGroupMap()
{
	// 이동
	m_iKeyGroupMap[DIK_W] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_A] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_S] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_D] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_UP] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_LEFT] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_DOWN] = KeyGroup::MOVEMENT;
	m_iKeyGroupMap[DIK_RIGHT] = KeyGroup::MOVEMENT;

	// 시스템
	m_iKeyGroupMap[DIK_F1] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F2] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F3] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F4] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F5] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F6] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F7] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F8] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F9] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F10] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F11] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F12] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_ESCAPE] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_P] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_O] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_F] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_T] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_B] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_RBRACKET] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_LBRACKET] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_BACKSLASH] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_END] = KeyGroup::SYSTEM;

	m_iKeyGroupMap[DIK_1] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_2] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_3] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_4] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_5] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_6] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_7] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_8] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_9] = KeyGroup::SYSTEM;
	m_iKeyGroupMap[DIK_0] = KeyGroup::SYSTEM;

	// UI 네비게이션
	m_iKeyGroupMap[DIK_RETURN] = KeyGroup::UI_NAVIGATE;
	m_iKeyGroupMap[DIK_TAB] = KeyGroup::UI_NAVIGATE;
	m_iKeyGroupMap[DIK_SPACE] = KeyGroup::UI_NAVIGATE;

	// 디버그용
	m_iKeyGroupMap[DIK_Z] = KeyGroup::TOOL;
	m_iKeyGroupMap[DIK_X] = KeyGroup::TOOL;
	m_iKeyGroupMap[DIK_C] = KeyGroup::TOOL;
}

CInput_Device* CInput_Device::Create(HINSTANCE hInstance, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

