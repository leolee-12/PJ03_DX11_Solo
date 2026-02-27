#pragma once

#include "UI_GameState.h"
#include <queue>

BEGIN(Client)

class CUI_KOParticle final :public CUI_GameState
{
private:
	CUI_KOParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_KOParticle(const CUI_KOParticle& Prototype);
	virtual ~CUI_KOParticle() = default;

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
	_float m_fTexCoord = { -0.5f };

public:
	static CUI_KOParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END