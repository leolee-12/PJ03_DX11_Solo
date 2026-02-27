#pragma once

#include "UI_Combo.h"

BEGIN(Client)

class CUI_ComboNumber final :public CUI_Combo
{
public:
	enum NUMBER {FIRST , SECOND ,THIRD ,NUMBER_END};


private:
	CUI_ComboNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ComboNumber(const CUI_ComboNumber& Prototype);
	virtual ~CUI_ComboNumber() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	void InitPosition();
	void ScaleAnimation();
	void EndAlphaEffect(_float fTimeDelta);
	_bool RenderEnable(_uint iLimit);
	_uint Return_TextureIndex();

private:
	_float m_fDepth = { 0.f };
	_float m_fAlphaTimer = { 0.f };
	_uint m_iNumUI = { 0 };
	
	_uint m_iLimitComboCount = { 0 };

	_bool m_bScaleAnim = { FALSE };
	_uint m_iPrevCombo = { 0 };


public:
	static CUI_ComboNumber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END