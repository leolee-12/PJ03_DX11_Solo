#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Level_Test_Defines.h"

/* 스레드를 생성한다. */
/* 생성한 스레드로 필요한 레벨의 자원을 로딩한다. */

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
	INFO_CLASS(CLoader, CBase)

private:
	CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLoader() = default;

public:

	_bool isFinished() const {
		return m_isFinished;
	}
		


public:	
	HRESULT Initialize(LEVEL eNextLevelID);

	void Print_LoadingText();



public:
	HRESULT Loading();
	HRESULT Loading_For_Logo_Level(LEVEL eLevelID);
	HRESULT Loading_For_GamePlay_Level(LEVEL eLevelID);
	HRESULT Loading_For_Tool_Level();


private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;

private:
	LEVEL					m_eNextLevelID = { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH] = TEXT("");
	_bool					m_isFinished = { false };

private:
	HRESULT					Ready_Effect_Texture();
	HRESULT					Ready_Effect_Model();

public:
	static CLoader * Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelID);
	virtual void Free() override;
	
};

END