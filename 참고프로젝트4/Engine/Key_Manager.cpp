#include "Key_Manager.h"
#include "GameInstance.h"
#include "Input_Device.h"

CKey_Manager::CKey_Manager()
{
}

HRESULT CKey_Manager::Initialize(CInput_Device* pInput_Device)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	m_pInput_Device = pInput_Device;

	for (_uint i = 0; i < 256; ++i)
		m_KeyState[i] = KEYSTATE::KEY_NONE;

	return S_OK;
}

void CKey_Manager::Update_KeyState()
{
	for (const auto& key : m_vecCheckKey)
	{
		if(m_pInput_Device->Key_Down(key))
			m_KeyState[key] = KEYSTATE::KEY_DOWN;
		else if (m_pInput_Device->Key_Up(key))
			m_KeyState[key] = KEYSTATE::KEY_UP;
		else  if (m_pInput_Device->Key_Pressing(key))
			m_KeyState[key] = KEYSTATE::KEY_PRESS;
		else
			m_KeyState[key] = KEYSTATE::KEY_NONE;
	}

	for (_uint i = 0; i < DIM_END; ++i)
	{
		if (m_pInput_Device->Mouse_Down((MOUSEKEYSTATE)i))
			m_MouseState[i] = KEYSTATE::KEY_DOWN;
		else if (m_pInput_Device->Mouse_Up((MOUSEKEYSTATE)i))
			m_MouseState[i] = KEYSTATE::KEY_UP;
		else  if (m_pInput_Device->Mouse_Pressing((MOUSEKEYSTATE)i))
			m_MouseState[i] = KEYSTATE::KEY_PRESS;
		else
			m_MouseState[i] = KEYSTATE::KEY_NONE;
	}
}

void CKey_Manager::Register_Key(_ubyte key)
{
	if(!Is_KeyRegistered(key))
		m_vecCheckKey.push_back(key);
}

_bool CKey_Manager::Is_KeyRegistered(_ubyte key)
{
	return find(m_vecCheckKey.begin(), m_vecCheckKey.end(), key) != m_vecCheckKey.end();
}

KEYSTATE CKey_Manager::Get_KeyState(_ubyte key)
{
	//if (!Is_KeyRegistered(key))
		//assert(false); //키 등록 필요

	return m_KeyState[key];
}

KEYSTATE CKey_Manager::Get_MouseState(MOUSEKEYSTATE mouse)
{
	return m_MouseState[mouse];
}

_bool CKey_Manager::Check_KeyState(_ubyte key, KEYSTATE keyState)
{
	//if (!Is_KeyRegistered(key))
		//assert(false); //키 등록 필요

	switch (keyState)
	{
	case KEYSTATE::KEY_UP:
			return m_KeyState[key] == KEYSTATE::KEY_UP;

	case KEYSTATE::KEY_DOWN:
		return m_KeyState[key] == KEYSTATE::KEY_DOWN;

	case KEYSTATE::KEY_PRESS:
		return m_KeyState[key] == KEYSTATE::KEY_DOWN || m_KeyState[key] == KEYSTATE::KEY_PRESS;
	default :
		return false;
	}
	return false;
}

_bool CKey_Manager::Check_MouseState(MOUSEKEYSTATE mouse, KEYSTATE keyState)
{
	switch (keyState)
	{
	case KEYSTATE::KEY_UP:
		return m_MouseState[mouse] == KEYSTATE::KEY_UP;

	case KEYSTATE::KEY_DOWN:
		return m_MouseState[mouse] == KEYSTATE::KEY_DOWN;

	case KEYSTATE::KEY_PRESS:
		return m_MouseState[mouse] == KEYSTATE::KEY_DOWN || m_MouseState[mouse] == KEYSTATE::KEY_PRESS;
	default:
		return false;
	}
	return false;
}

CKey_Manager* CKey_Manager::Create(CInput_Device* pInput_Device)
{
	CKey_Manager* pInstance = new CKey_Manager;

	if (FAILED(pInstance->Initialize(pInput_Device)))
	{
		MSG_BOX("Failed to Created : CKey_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKey_Manager::Free()
{
	m_vecCheckKey.clear();
	Safe_Release(m_pGameInstance);
}
