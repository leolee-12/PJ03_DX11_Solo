#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_CharaSelectMark final :public CUIObject
{
private:
	CUI_CharaSelectMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharaSelectMark(const CUI_CharaSelectMark& Prototype);
	virtual ~CUI_CharaSelectMark() = default;

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
	static CUI_CharaSelectMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END