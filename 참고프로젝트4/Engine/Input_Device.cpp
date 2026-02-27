#include "Input_Device.h"
#include "PipeLine.h"
#include "GameInstance.h"

Engine::CInput_Device::CInput_Device(void)
{
	ZeroMemory(m_byKeyInputState, sizeof(m_byKeyInputState));
}

Engine::CInput_Device::~CInput_Device(void)
{
	Free();
}


HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		RETURN_EFAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		RETURN_EFAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		RETURN_EFAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	m_hWnd = hWnd;
	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void Engine::CInput_Device::Update_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

bool CInput_Device::Key_Pressing(_ubyte byKeyID)
{
	if (!CanUpdate()) { return false; }

	if (m_byKeyState[byKeyID] & 0x80)
	{
		if (m_byKeyInputState[byKeyID] <= EInputState::Release)
			m_byKeyInputState[byKeyID] = EInputState::Pressed;
	}

	return m_byKeyInputState[byKeyID] >= EInputState::Pressing;
}

bool CInput_Device::Key_Down(_ubyte byKeyID)
{
	if (!CanUpdate()) { return false; }

	// 이전에는 눌린 적이 없고 현재 눌렸을 경우
	if (m_byKeyInputState[byKeyID] <= EInputState::Release && (m_byKeyState[byKeyID] & 0x80))
		m_byKeyInputState[byKeyID] = EInputState::Pressed; //true

	return (m_byKeyInputState[byKeyID] == EInputState::Pressed);
}

bool CInput_Device::Key_Up(_ubyte byKeyID)
{
	if (!CanUpdate()) { return false; }

	// 이전에는 눌린 적이 있고 현재 눌리지 않았을 경우
	if (m_byKeyInputState[byKeyID] >= EInputState::Pressing && !(m_byKeyState[byKeyID] & 0x80))
		m_byKeyInputState[byKeyID] = EInputState::Released;

	return (m_byKeyInputState[byKeyID] == EInputState::Released);
}

bool CInput_Device::Key_PressingEx(_ubyte byKeyID, DIK_EX byModifierType)
{
	if (!CanUpdate()) { return false; }

	_bool bIsModifierActive = Modifier_Check(byModifierType);
	if (!bIsModifierActive)
		return false;

	if (m_byKeyState[byKeyID] & 0x80)
	{
		if (m_byKeyInputState[byKeyID] <= EInputState::Release)
			m_byKeyInputState[byKeyID] = EInputState::Pressed;
	}

	return m_byKeyInputState[byKeyID] >= EInputState::Pressing;
}

bool CInput_Device::Key_DownEx(_ubyte byKeyID, DIK_EX byModifierType)
{
	if (!CanUpdate()) { return false; }

	_bool bIsModifierActive = Modifier_Check(byModifierType);
	if (!bIsModifierActive)
		return false;

	// 이전에는 눌린 적이 없고 현재 눌렸을 경우
	if (m_byKeyInputState[byKeyID] <= EInputState::Release && (m_byKeyState[byKeyID] & 0x80))
		m_byKeyInputState[byKeyID] = EInputState::Pressed; //true

	return (m_byKeyInputState[byKeyID] == EInputState::Pressed);
}

bool CInput_Device::Key_UpEx(_ubyte byKeyID, DIK_EX byModifierType)
{
	if (!CanUpdate()) { return false; }

	_bool bIsModifierActive = Modifier_Check(byModifierType);
	if (!bIsModifierActive)
		return false;

	// 이전에는 눌린 적이 있고 현재 눌리지 않았을 경우
	if (m_byKeyInputState[byKeyID] >= EInputState::Pressing && !(m_byKeyState[byKeyID] & 0x80))
		m_byKeyInputState[byKeyID] = EInputState::Released;

	return (m_byKeyInputState[byKeyID] == EInputState::Released);
}

_bool CInput_Device::Modifier_Check(const DIK_EX& byModifierType)
{
	switch (byModifierType)
	{
	case DIK_MD_NONE:
		if (!(m_byKeyState[DIK_LCONTROL] & 0x80) && !(m_byKeyState[DIK_LSHIFT] & 0x80) && !(m_byKeyState[DIK_LALT] & 0x80))
			return true;
		break;
	case DIK_MD_LCTRL:
		if (m_byKeyState[DIK_LCONTROL] & 0x80 && !(m_byKeyState[DIK_LSHIFT] & 0x80) && !(m_byKeyState[DIK_LALT] & 0x80))
			return true;
		break;
	case DIK_MD_LSHIFT:
		if (m_byKeyState[DIK_LSHIFT] & 0x80 && !(m_byKeyState[DIK_LCONTROL] & 0x80) && !(m_byKeyState[DIK_LALT] & 0x80))
			return true;
		break;
	case DIK_MD_LALT:
		if (m_byKeyState[DIK_LALT] & 0x80 && !(m_byKeyState[DIK_LCONTROL] & 0x80) && !(m_byKeyState[DIK_LSHIFT] & 0x80))
			return true;
		break;
	case DIKK_MD_LCTRL_SHIFT:
		if (m_byKeyState[DIK_LCONTROL] & 0x80 && m_byKeyState[DIK_LSHIFT] & 0x80 && !(m_byKeyState[DIK_LALT] & 0x80))
			return true;
		break;
	case DIK_MD_LCTRL_ALT:
		if (m_byKeyState[DIK_LCONTROL] & 0x80 && m_byKeyState[DIK_LALT] & 0x80 && !(m_byKeyState[DIK_LSHIFT] & 0x80))
			return true;
		break;
	case DIKK_MD_LSHIFT_ALT:
		if (m_byKeyState[DIK_LSHIFT] & 0x80 && m_byKeyState[DIK_LALT] & 0x80 && !(m_byKeyState[DIK_LCONTROL] & 0x80))
			return true;
		break;
	case DIK_MD_LCTRL_SHIFT_ALT:
		if (m_byKeyState[DIK_LCONTROL] & 0x80 && m_byKeyState[DIK_LSHIFT] & 0x80 && m_byKeyState[DIK_LALT])
			return true;
		break;
	}

	return false;
}

_bool CInput_Device::Any_Input_Received()
{
	for (int i = 0; i < 256; ++i)
	{
		if (m_byKeyState[i] & 0x80)
			return true; 
	}

	for (int i = 0; i < DIM_END; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
			return true; 
	}

	return false; // 아무 입력도 없음
}

_bool CInput_Device::CanUpdate()
{
	return CGameInstance::m_bIsFocusedWindow;
}

bool CInput_Device::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	if (!CanUpdate()) { return false; }

	if (eMouse < 0 || eMouse >= MOUSEKEYSTATE::DIM_END)
		return false;

	if (m_tMouseState.rgbButtons[eMouse] & 0x80)
	{
		if (m_tMouseInputState[eMouse] <= EInputState::Release)
			m_tMouseInputState[eMouse] = EInputState::Pressed;
	}

	return (m_tMouseInputState[eMouse] >= EInputState::Pressing);
}

bool CInput_Device::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	if (!CanUpdate()) { return false; }

	if (eMouse < 0 || eMouse >= MOUSEKEYSTATE::DIM_END)
		return false;

	// 이전에는 눌린 적이 없고 현재 눌렸을 경우
	if (m_tMouseState.rgbButtons[eMouse] & 0x80)
	{
		if (m_tMouseInputState[eMouse] <= EInputState::Release)
			m_tMouseInputState[eMouse] = EInputState::Pressed; //true
	}

	return (m_tMouseInputState[eMouse] == EInputState::Pressed);
}

bool CInput_Device::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	if (!CanUpdate()) { return false; }

	if (eMouse < 0 || eMouse >= MOUSEKEYSTATE::DIM_END)
		return false;

	// 이전에는 눌린 적이 있고 현재 눌리지 않았을 경우
	if (!(m_tMouseState.rgbButtons[eMouse] & 0x80))
	{
		if (m_tMouseInputState[eMouse] >= EInputState::Pressing)
			m_tMouseInputState[eMouse] = EInputState::Released;
	}

	return (m_tMouseInputState[eMouse] == EInputState::Released);
}

void CInput_Device::Mouse_Fix()
{
	POINT	pt{ (_long)m_iWinSizeX >> 1, (_long)m_iWinSizeY >> 1 }; //화면 중앙

	ClientToScreen(m_hWnd, &pt); //Screen좌표 ?
	SetCursorPos(pt.x, pt.y); //화면 중앙으로 마우스 커서 위치 고정
}

_bool CInput_Device::Mouse_RayIntersect(SPACETYPE eSpacetype, _float3* pOut, _fvector vV1, _fvector vV2, _fvector vV3, _matrix matWorld, _float* pLengthOut)
{
	_vector vOrigin, vDir;
	Mouse_ToWorld(vOrigin, vDir);
	
	_float fDist = 0.f;

	_vector vPickPos;
	_vector	vRayPos = XMVector3TransformCoord(vOrigin, XMMatrixInverse(nullptr, matWorld));
	_vector	vRayDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMMatrixInverse(nullptr, matWorld)));

	if (DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vV1, vV2, vV3, fDist))
	{
		vPickPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;

		if(eSpacetype == SP_WORLD)
			vPickPos = XMVector3TransformCoord(vPickPos, matWorld);
		XMStoreFloat3(pOut, vPickPos);

		if (pLengthOut != nullptr)
		{
			//_float3 fTemp;
			//XMStoreFloat3(&fTemp, XMVector3Length(vRayDir));
			*pLengthOut = fDist;
		}

		return true;
	}

	return false;
}

void CInput_Device::Mouse_ToWorld(_vector& vOrigin, _vector& vDir)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	POINT	ptCursor;
	RECT	rcClient;
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);
	GetClientRect(m_hWnd, &rcClient);

	_matrix	matView = pGameInstance->Get_TransformMatrix(CPipeLine::TRANSFORMSTATE::TS_VIEW);
	_matrix	matProj = pGameInstance->Get_TransformMatrix(CPipeLine::TRANSFORMSTATE::TS_PROJ);

	_float3		vCursor(2.f * ptCursor.x / rcClient.right - 1.f, -2.f * ptCursor.y / rcClient.bottom + 1.f, 0.f);

	vOrigin = XMVector3TransformCoord(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMMatrixInverse(nullptr, matView));
	vDir = XMVector3Normalize(XMVector3TransformNormal(XMVector3TransformCoord(XMLoadFloat3(&vCursor), XMMatrixInverse(nullptr, matProj)),
		XMMatrixInverse(nullptr, matView)));
}

void CInput_Device::LateUpdate_InputDev(void)
{
	// 키보드
	for (int i = 0; i < 256; ++i) //눌린적 있고 현재 눌리지 않은 것 false로?
	{
		// 떼고 있음
		if (!(m_byKeyState[i] & 0x80))
		{
			if (m_byKeyInputState[i] >= EInputState::Pressing)
				m_byKeyInputState[i] = EInputState::Released;
			else if (m_byKeyInputState[i] == EInputState::Released)
				m_byKeyInputState[i] = EInputState::Release;
		}
		// 누르고 있음
		else if (m_byKeyState[i] & 0x80)
		{
			if (m_byKeyInputState[i] <= EInputState::Release)
				m_byKeyInputState[i] = EInputState::Pressed;
			else if (m_byKeyInputState[i] == EInputState::Pressed)
				m_byKeyInputState[i] = EInputState::Pressing;
		}
	}

	// 마우스
	for (int i = 0; i < DIM_END; ++i) //눌린적 있고 현재 눌리지 않은 것 false로?
	{
		// 떼고 있음
		if (!(m_tMouseState.rgbButtons[i] & 0x80))
		{
			if (m_tMouseInputState[i] >= EInputState::Pressing)
				m_tMouseInputState[i] = EInputState::Released;
			else if (m_tMouseInputState[i] == EInputState::Released)
				m_tMouseInputState[i] = EInputState::Release;
		}
		// 누르고 있음
		else if ((m_tMouseState.rgbButtons[i] & 0x80))
		{
			if ((_int)m_tMouseInputState[i] <= (_int)EInputState::Release)
				m_tMouseInputState[i] = EInputState::Pressed;
			else if (m_tMouseInputState[i] == EInputState::Pressed)
				m_tMouseInputState[i] = EInputState::Pressing;
		}
	}
}


CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
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

