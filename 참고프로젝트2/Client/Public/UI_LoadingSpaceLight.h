#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_LoadingSpaceLight final :public CUIObject
{
private:
	CUI_LoadingSpaceLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LoadingSpaceLight(const CUI_LoadingSpaceLight& Prototype);
	virtual ~CUI_LoadingSpaceLight() = default;

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
	void AlphaSwiching(_float& fAlphaValue, _float fTimeDelta);
	void ScaleAnim();

private:
	_float m_fAlphaValue = { 0.f };
	_bool m_IsAlphaSwich{ FALSE };

public:
	static CUI_LoadingSpaceLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END