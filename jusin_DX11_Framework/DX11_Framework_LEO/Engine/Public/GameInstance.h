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
	void	Clear_Resources(_int iLevelIndex);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	float	Compute_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT	Change_Level(_int iNewLevelIndex, class CLevel* pNewLevel);
#pragma endregion

private:
	class CGraphic_Device*	m_pGraphic_Device = { nullptr };
	class CTimer_Manager*	m_pTimer_Manager = { nullptr };
	class CLevel_Manager*	m_pLevel_Manager = { nullptr };

public:
	virtual void Free() override;
};

NS_END