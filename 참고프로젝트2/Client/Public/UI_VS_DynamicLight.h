#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_VS_DynamicLight final :public CUIObject
{
private:
	CUI_VS_DynamicLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_DynamicLight(const CUI_VS_DynamicLight& Prototype);
	virtual ~CUI_VS_DynamicLight() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

public:
	static CUI_VS_DynamicLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END