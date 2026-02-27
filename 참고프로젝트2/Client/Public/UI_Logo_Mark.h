#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Logo_Mark final :public CUIObject
{
private:
	CUI_Logo_Mark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Logo_Mark(const CUI_Logo_Mark& Prototype);
	virtual ~CUI_Logo_Mark() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();
	void PressingCreateFont(_float fTimeDelta);

private:
	_float m_fAlphaValue = { 1.f };
	_bool m_IsAlphaSwich = { FALSE };

public:
	static CUI_Logo_Mark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END