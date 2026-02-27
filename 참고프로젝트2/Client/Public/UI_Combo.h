#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Combo abstract : public CUIObject
{
public:
	enum ComboColor { RED, BLUE, PURPLE, COLOR_END };

protected:
	CUI_Combo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Combo(const CUI_Combo& Prototype);
	virtual ~CUI_Combo() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Ready_Components();

public:
	_uint SetColor();

protected:
	_bool m_bComboEnd = { FALSE };
	_bool m_bInitHit = { FALSE };

	_uint m_iComboCount = { 0 };
	_float m_fAlphaTimer = { 0.f };

	_float m_fDestoryTimer = { 0.f };

	_bool m_bComboHit = { FALSE };

	ComboColor m_eColorVaule = { COLOR_END };
	

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

