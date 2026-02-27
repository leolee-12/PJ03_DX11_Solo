#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Loading_Font final :public CUIObject
{
private:
	CUI_Loading_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_Font(const CUI_Loading_Font& Prototype);
	virtual ~CUI_Loading_Font() = default;

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
	static CUI_Loading_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END