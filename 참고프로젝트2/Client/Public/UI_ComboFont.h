#pragma once

#include "UI_Combo.h"

BEGIN(Client)

class CUI_ComboFont final :public CUI_Combo
{
private:
	CUI_ComboFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ComboFont(const CUI_ComboFont& Prototype);
	virtual ~CUI_ComboFont() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

public:
	static CUI_ComboFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END