#include "..\Public\Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{
}

HRESULT Engine::CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
    // DInput 컴객체를 생성하는 함수
    if (FAILED(DirectInput8Create(hInst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_pInputSDK,
        NULL)))
        return E_FAIL;

    // 키보드 객체 생성
    if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
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

    return S_OK;
}

void Engine::CInput_Device::Update(void)
{
	// 이전 키 상태를 저장합니다.
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(m_byKeyState));

	// 현재 키 상태를 가져옵니다.
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	// 이전 마우스 상태를 저장합니다.
	for (int i = 0; i < 3; ++i) {
		m_bPrevMouseState[i] = m_bMouseState[i];
	}

	// 현재 마우스 상태를 가져옵니다.
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	// 현재 프레임의 마우스 상태를 저장합니다.
	for (int i = 0; i < 3; ++i) {
		m_bMouseState[i] = (m_tMouseState.rgbButtons[i] & 0x80) != 0;
	}
}

_bool CInput_Device::Key_Pressing(_uint _iKey) {
	return (m_byKeyState[_iKey] & 0x80) != 0;
}

_bool CInput_Device::Key_Down(_uint _iKey) {
	return (!(m_byPrevKeyState[_iKey] & 0x80) && (m_byKeyState[_iKey] & 0x80));
}

_bool CInput_Device::Key_Up(_uint _iKey) {
	return ((m_byPrevKeyState[_iKey] & 0x80) && !(m_byKeyState[_iKey] & 0x80));
}

_bool CInput_Device::Mouse_Pressing(_uint _iButton) {
	return m_bMouseState[_iButton];
}

_bool CInput_Device::Mouse_Down(_uint _iButton) {
	return m_bMouseState[_iButton] && !m_bPrevMouseState[_iButton];
}

_bool CInput_Device::Mouse_Up(_uint _iButton) {
	return !m_bMouseState[_iButton] && m_bPrevMouseState[_iButton];
}

CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
    CInput_Device* pInstance = new CInput_Device();

    if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
    {
        MSG_BOX(TEXT("Failed to Created : CInput_Device"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Engine::CInput_Device::Free(void)
{
    Safe_Release(m_pKeyBoard);
    Safe_Release(m_pMouse);
    Safe_Release(m_pInputSDK);
}

