#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CInput_Device;

class ENGINE_DLL CKey_Manager final : public CBase
{
	INFO_CLASS(CKey_Manager, CBase)

private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	HRESULT Initialize(CInput_Device* pInput_Device);
	void Update_KeyState();

private:
	CGameInstance* m_pGameInstance = nullptr;
	CInput_Device* m_pInput_Device = nullptr;

	KEYSTATE m_KeyState[256];
	vector< _ubyte> m_vecCheckKey;
	KEYSTATE m_MouseState[DIM_END];

public:
	void Register_Key(_ubyte key);
	_bool Is_KeyRegistered(_ubyte key);
	KEYSTATE Get_KeyState(_ubyte key);
	KEYSTATE Get_MouseState(MOUSEKEYSTATE mouse);
	_bool    Check_KeyState(_ubyte key, KEYSTATE keyState);
	_bool	 Check_MouseState(MOUSEKEYSTATE mouse, KEYSTATE keyState);


public:
	static CKey_Manager* Create(CInput_Device* pInput_Device);
	virtual void Free() override;
};


END

