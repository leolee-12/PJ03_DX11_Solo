#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_KOFontEffect final :public CUI_GameState
{
private:
	CUI_KOFontEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_KOFontEffect(const CUI_KOFontEffect& Prototype);
	virtual ~CUI_KOFontEffect() = default;

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
	_float m_fScaleAnim = { 0 };
	_float m_fAlphaTimer = { 0.f };
public:
	static CUI_KOFontEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END