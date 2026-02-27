#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_KOPanel final :public CUI_GameState
{
private:
	CUI_KOPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_KOPanel(const CUI_KOPanel& Prototype);
	virtual ~CUI_KOPanel() = default;

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
	_bool m_bDeadCheck = {FALSE};

	_float m_fDestroyTimer = { 0.f };
	_float m_fLightTimer = { 0.f };
	
public:
	static CUI_KOPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END