#pragma once

#include "Client_Defines.h"
#include "Level.h"


/* 로딩 레벨에 보여주고 싶은장면을 갱신하고 보여준다. */

/* m_eNextLevelID에 따른 레벨에 핑룡한 자원을 주비하 기위한 작업를 하는 스레드를 생성해준다. . */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
	INFO_CLASS(CLevel_Loading, CLevel)

private:
	CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL			m_eNextLevelID = { LEVEL_END };
	class CLoader*	m_pLoader = { nullptr };
	
	shared_ptr<class CUI_LoadingLogo> m_pLoadingUI = { nullptr };
	shared_ptr<class CLight > m_pLight;
	
public:
	static CLevel_Loading* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END