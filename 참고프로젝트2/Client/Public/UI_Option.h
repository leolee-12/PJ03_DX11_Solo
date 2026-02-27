#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Option abstract : public CUIObject
{
protected:
	CUI_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Option(const CUI_Option& Prototype);
	virtual ~CUI_Option() = default;

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
	void OnSwitch(_bool bSwitch) { m_bCreateSwitch = bSwitch; }

private:
	void InitAnimation(_float fTimeDelta);

private:
	_bool m_bCreateSwitch = { FALSE };
	_bool m_bInitData = { FALSE };
	_float m_fOriginPos = { 0.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

