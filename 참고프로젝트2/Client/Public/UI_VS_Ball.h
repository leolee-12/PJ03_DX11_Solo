#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_VS_Ball final :public CUIObject
{
private:
	CUI_VS_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_Ball(const CUI_VS_Ball& Prototype);
	virtual ~CUI_VS_Ball() = default;

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
	static CUI_VS_Ball* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END