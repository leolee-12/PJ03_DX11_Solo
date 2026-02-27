#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_CharaSelectFont final :public CUIObject
{
private:
	CUI_CharaSelectFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharaSelectFont(const CUI_CharaSelectFont& Prototype);
	virtual ~CUI_CharaSelectFont() = default;

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
	static CUI_CharaSelectFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END