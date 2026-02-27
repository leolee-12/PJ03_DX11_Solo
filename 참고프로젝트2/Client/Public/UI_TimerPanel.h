#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_TimerPanel final :public CUIObject
{
private:
	CUI_TimerPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TimerPanel(const CUI_TimerPanel& Prototype);
	virtual ~CUI_TimerPanel() = default;

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
	static CUI_TimerPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END