#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Loading_CreateFlyEff final :public CUIObject
{
private:
	CUI_Loading_CreateFlyEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_CreateFlyEff(const CUI_Loading_CreateFlyEff& Prototype);
	virtual ~CUI_Loading_CreateFlyEff() = default;

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
	void ScaleAnim(_float fTimeDelta);

private:
	_float m_fScaleValue = { 1.f };
	_float m_fAlphaValue = { 1.f };

public:
	static CUI_Loading_CreateFlyEff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END