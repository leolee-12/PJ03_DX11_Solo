#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_KOFont final :public CUI_GameState
{
private:
	CUI_KOFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_KOFont(const CUI_KOFont& Prototype);
	virtual ~CUI_KOFont() = default;

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
	_float m_fAlphaValue = { 1.f };

public:
	static CUI_KOFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END