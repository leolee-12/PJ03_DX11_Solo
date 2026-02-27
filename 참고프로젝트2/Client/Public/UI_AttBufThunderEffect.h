#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_AttBufThunderEffect final :public CUI_BaseAttBuf
{
private:
	CUI_AttBufThunderEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_AttBufThunderEffect(const CUI_AttBufThunderEffect& Prototype);
	virtual ~CUI_AttBufThunderEffect() = default;

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
	_float m_fAnimFrame = { 0.f };

public:
	static CUI_AttBufThunderEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END