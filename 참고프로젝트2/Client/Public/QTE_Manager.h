#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"
#include "QTE_Hit.h"
BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CQTE_Manager : public CBase
{
	DECLARE_SINGLETON(CQTE_Manager)

public:
	enum QTE_ID
	{
		QTE_ID_SAME_GRAB,
		QTE_ID_HIT,
		QTE_ID_CONTINUOUS_ATTACK,
		QTE_ID_1P_SAME_GRAB,
		QTE_ID_END
	};

private:
	CQTE_Manager();
	virtual ~CQTE_Manager() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

	CGameInstance* m_pGameInstance = { nullptr };

	vector<class CGameObject*> m_vecQTE;

	void Start_QTE(QTE_ID ID, class CGameObject* callObject = nullptr, _float _lifeTime = -1.f);

public:
	virtual void Free() override;
};

END