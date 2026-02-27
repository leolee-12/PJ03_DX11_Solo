#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_StartEmblem final :public CUI_GameState
{
private:
	CUI_StartEmblem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StartEmblem(const CUI_StartEmblem& Prototype);
	virtual ~CUI_StartEmblem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

public:
	static CUI_StartEmblem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END