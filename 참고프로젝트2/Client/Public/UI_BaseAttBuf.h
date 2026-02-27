#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_BaseAttBuf abstract : public CUIObject
{
protected:
	CUI_BaseAttBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BaseAttBuf(const CUI_BaseAttBuf& Prototype);
	virtual ~CUI_BaseAttBuf() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

public:
	UI_LRPOS Get_UIPos() { return m_eLRPos; }

private:
	_uint m_iUseCount = { 1 };
	_float m_fAttBufTimer = { 0.f };
	_float m_fAttBufDuration = { 0.f };

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Ready_Components();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

