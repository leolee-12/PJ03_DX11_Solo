#pragma once

#include "UI_Combo.h"

BEGIN(Client)

class CUI_ComboEffect final :public CUI_Combo
{
private:
	CUI_ComboEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ComboEffect(const CUI_ComboEffect& Prototype);
	virtual ~CUI_ComboEffect() = default;

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
	_float m_fAnimDuration = { 0.f };

public:
	static CUI_ComboEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END