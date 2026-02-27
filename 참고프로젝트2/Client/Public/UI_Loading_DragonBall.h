#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Loading_DragonBall final :public CUIObject
{
private:
	CUI_Loading_DragonBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_DragonBall(const CUI_Loading_DragonBall& Prototype);
	virtual ~CUI_Loading_DragonBall() = default;

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

	void InitPosition();

private:
	_bool m_bSwitchColor = { FALSE };
	_bool m_bFinishEnd = { FALSE };
	_bool m_bFinishAnim = { FALSE };

	_uint m_iTextureIndex = { 0 };
	_uint m_iDefTextureIndex = { 7 };

	_float m_fBlurValue = { 0.f };
	_bool m_bBlurSwitch = { FALSE };

	_float m_fMaskValue = { 0.f };
	_bool m_bMaskValueSwitch = { FALSE };

public:
	static CUI_Loading_DragonBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END