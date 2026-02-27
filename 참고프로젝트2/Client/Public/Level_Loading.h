#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 1. 로딩 장면에 필요한 객체를 만들고, 리소스를 로드한다. */
/* 2. 로딩 장면을 유저에게 보여준다. */


BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Layer();

	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	HRESULT Ready_Sound();

private:
	LEVELID				m_eNextLevelID = {};
	class CLoader*		m_pLoader = { nullptr };
	
	_float m_fNextLevelTimer = { 0.f };
	_uint m_iNumThreadEnd = { 0 };

	_bool m_bNextLevel = { FALSE };
		
	static _bool m_bIsLevelPrepared;

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END