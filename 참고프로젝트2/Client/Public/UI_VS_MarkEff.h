#pragma once

#include "UI_GameState.h"

BEGIN(Client)

class CUI_VS_MarkEff final :public CUI_GameState
{
private:
	CUI_VS_MarkEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_MarkEff(const CUI_VS_MarkEff& Prototype);
	virtual ~CUI_VS_MarkEff() = default;

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
	_bool m_bDrawSwich = { FALSE };
	_float m_fAlphaValue = { 1.f };

public:
	static CUI_VS_MarkEff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END