#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_CharaSelectLight final :public CUIObject
{
private:
	CUI_CharaSelectLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharaSelectLight(const CUI_CharaSelectLight& Prototype);
	virtual ~CUI_CharaSelectLight() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

	void ScaledAnimation(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

private:
	_float m_fScaledValue = { 0.f };
	_bool m_bValueSwitch = { FALSE };
	_uint m_iTextureIndex = { 0 };
	
public:
	static CUI_CharaSelectLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END