#pragma once

#include "UI_BaseAttBuf.h"

BEGIN(Client)

class CUI_VS_BoltEff final :public CUIObject
{
private:
	CUI_VS_BoltEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_VS_BoltEff(const CUI_VS_BoltEff& Prototype);
	virtual ~CUI_VS_BoltEff() = default;

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
	_float Get_RandomNormalize();
	_float Get_Random(_float fMin, _float fMax);
	_float Get_RotationValue(_vector CurrPos);
	_float Get_Alpha();

private:
	_float m_fAnimDuration = { 0.f };
	_float m_fAlphaValue = { 0.f };

public:
	static CUI_VS_BoltEff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END