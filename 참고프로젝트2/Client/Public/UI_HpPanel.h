#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_HpPanel final :public CUIObject
{
private:
	CUI_HpPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_HpPanel(const CUI_HpPanel& Prototype);
	virtual ~CUI_HpPanel() = default;

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
	static CUI_HpPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END