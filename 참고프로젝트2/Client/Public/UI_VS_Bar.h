#pragma once

#include "UIObject.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CUI_VS_Bar final :public CUIObject
{
private:
	CUI_VS_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_Bar(const CUI_VS_Bar& Prototype);
	virtual ~CUI_VS_Bar() = default;

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
	_uint iNumUI = { 0 };

	CTexture* m_pMaskTexture = { nullptr };

public:
	static CUI_VS_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END