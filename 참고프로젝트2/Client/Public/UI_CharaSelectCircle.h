#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_CharaSelectCircle final :public CUIObject
{
private:
	CUI_CharaSelectCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharaSelectCircle(const CUI_CharaSelectCircle& Prototype);
	virtual ~CUI_CharaSelectCircle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	void Play_Animation(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

private:
	_float m_fScaledValue = { 0.f };
	_bool m_bValueSwitch = { FALSE };

public:
	static CUI_CharaSelectCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END