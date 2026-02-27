#pragma once

#include "UI_BaseAttBuf.h"


BEGIN(Engine)

class CVIBuffer_Trail_Rect;

END

BEGIN(Client)


class CUI_Loading_EnergyEff final :public CUIObject
{
private:
	CUI_Loading_EnergyEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_EnergyEff(const CUI_Loading_EnergyEff& Prototype);
	virtual ~CUI_Loading_EnergyEff() = default;

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
	CVIBuffer_Trail_Rect* m_pTrailVIBuffer = { nullptr };

private:
	_float m_fAngle = { 0.f };

public:
	static CUI_Loading_EnergyEff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END