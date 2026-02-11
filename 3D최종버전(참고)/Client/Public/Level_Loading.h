#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 1. 다음레벨에 대한 자원(텍스쳐, 모델, 사운드, 객체원형)을 준비한다. -> CLoader객체에게 하청 */
/* 2. 로딩레벨을 구성해주기위한 객체생성과 업데이트와 렌더호출통해 동적인 로딩화면을 구성해준다. */

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL			m_eNextLevelID = { LEVEL::END };
	class CLoader* m_pLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_UI();

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END