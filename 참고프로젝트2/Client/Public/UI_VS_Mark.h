#pragma once

#include "UI_GameState.h"

BEGIN(Client)

class CUI_VS_Mark final :public CUI_GameState
{
private:
	CUI_VS_Mark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_Mark(const CUI_VS_Mark& Prototype);
	virtual ~CUI_VS_Mark() = default;

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
	_float m_fAlphaValue = { 0.f };

public:
	static CUI_VS_Mark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END