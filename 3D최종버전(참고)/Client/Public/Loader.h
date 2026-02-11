#pragma once

/* 1. 다음레벨에 대한 자원(텍스쳐, 모델, 사운드, 객체원형)을 준비한다.*/
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	_bool isFinished() const {
		return m_isFinished;
	}

	void Print_Text();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };

private:
	_bool						m_isFinished = { false };
	LEVEL						m_eNextLevelID = {};
	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};
	_tchar						m_szLoading[128] = {};

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();


	
public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END