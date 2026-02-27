#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_SubHpPanel final :public CUIObject
{
private:
	CUI_SubHpPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SubHpPanel(const CUI_SubHpPanel& Prototype);
	virtual ~CUI_SubHpPanel() = default;

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

	_float m_fDebug = { 0.1f };

public:
	static CUI_SubHpPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END