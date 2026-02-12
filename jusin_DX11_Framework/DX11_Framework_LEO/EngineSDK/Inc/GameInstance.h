#pragma once
#include "Base.h"

/* ------------------------------------------------------------ */
// CGameInstance : 클라이언트에게 엔진 기능을 위한 메서드 제공
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	HRESULT	Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void	Update_Engine(_float fTimeDelta);
	HRESULT	Begin_Draw();
	HRESULT	Draw();
	HRESULT	End_Draw();
#pragma endregion

#pragma region TIMER_MANAGER
public:
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	float	Compute_Timer(const _wstring& strTimerTag);
#pragma endregion

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };

public:
	virtual void Free() override;
};

NS_END