#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Win_Circle final :public CUIObject
{
private:
	CUI_Win_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Win_Circle(const CUI_Win_Circle& Prototype);
	virtual ~CUI_Win_Circle() = default;

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
	_float m_fDebugRatio = { 0.1f };

	_float m_fDestroy = { 0.f };
	_float m_fAngle = { 0.f };
public:
	static CUI_Win_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END