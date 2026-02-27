#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_VS_NameOutLine final :public CUIObject
{
private:
	CUI_VS_NameOutLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_NameOutLine(const CUI_VS_NameOutLine& Prototype);
	virtual ~CUI_VS_NameOutLine() = default;

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
	_float m_fRotaionValue = { 0.f };
	_float m_fTotalNum = { 0.f };
	_uint m_iNumUI = { 0 };

public:
	static CUI_VS_NameOutLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END