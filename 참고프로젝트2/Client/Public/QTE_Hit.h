#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CQTE_Hit final : public CGameObject
{
public:

	enum Hit_Situation_ID
	{
		Hit_Situation_ID_Goku,
		Hit_Situation_ID_21,
		Hit_Situation_ID_Frieza,
		Hit_Situation_ID_Hit,
		Hit_Situation_ID_END
	};

	struct QTE_HIT_DESC
	{
		CQTE_Hit::Hit_Situation_ID ID = {};
	};

private:
	CQTE_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Hit(const CQTE_Hit& Prototype);
	virtual ~CQTE_Hit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Start_Hit(class CGameObject* pCall_Object);

private:
	vector<class CQTE_Hit_Situation* >m_vecHit_Situation;

	Hit_Situation_ID m_current_Situation_ID = Hit_Situation_ID_END;
	_bool m_bIsQTEActive = { false }; // QTE 활성화 여부

	//QTE 호출자
	class CGameObject* m_pCall_Object = { nullptr };
public:
	static CQTE_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END