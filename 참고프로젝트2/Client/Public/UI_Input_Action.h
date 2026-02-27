#pragma once

#include "UI_Input.h"

BEGIN(Client)

class CUI_Input_Action final :public CUI_Input
{
public:
	enum ACTION_KEY {KEY_LIGHT , KEY_MEDIUM , KEY_SPECIAL, KEY_HEAVY , KEY_A1,KEY_A2 , KEY_END};
private:
	CUI_Input_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Input_Action(const CUI_Input_Action& Prototype);
	virtual ~CUI_Input_Action() = default;

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
	_bool m_bOnBtn = { FALSE };
	_uint m_iTextureIndex = { 0 };
	_float m_fColor = { 0.f };

	_float m_fBtnOffTimer = { 0.f };

public:
	static CUI_Input_Action* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END