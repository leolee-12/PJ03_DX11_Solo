#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_HpEffect final :public CUIObject
{
private:
	CUI_HpEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_HpEffect(const CUI_HpEffect& Prototype);
	virtual ~CUI_HpEffect() = default;

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
	_float m_fAlphaTimer = { 0.f };
	_bool m_bSign = { TRUE };

public:
	static CUI_HpEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END