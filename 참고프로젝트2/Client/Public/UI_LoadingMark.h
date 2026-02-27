#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_LoadingMark final :public CUIObject
{
private:
	CUI_LoadingMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LoadingMark(const CUI_LoadingMark& Prototype);
	virtual ~CUI_LoadingMark() = default;

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
	_uint m_iTextureIndex = { 0 };

public:
	static CUI_LoadingMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END