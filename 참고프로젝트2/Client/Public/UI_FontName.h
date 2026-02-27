#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_FontName final :public CUIObject
{
private:
	CUI_FontName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FontName(const CUI_FontName& Prototype);
	virtual ~CUI_FontName() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

private:
	_uint m_iTexIdx = { 0 };

public:
	static CUI_FontName* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END