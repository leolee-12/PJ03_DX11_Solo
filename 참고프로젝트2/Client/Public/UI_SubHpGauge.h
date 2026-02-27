#pragma once

#include "UIObject.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CUI_SubHpGauge final :public CUIObject
{
private:
	CUI_SubHpGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SubHpGauge(const CUI_SubHpGauge& Prototype);
	virtual ~CUI_SubHpGauge() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	_char m_iShaderID = { 0 };

	_float m_iCharaCurrHp = { 0.f };
	_float m_fHpRadio = { 0.f };
	_float m_fRedHpRadio = { 0.f };

	_float m_fOffset = { 0.1f };
public:
	static CUI_SubHpGauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END