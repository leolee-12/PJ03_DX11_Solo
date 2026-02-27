#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_PosMark final :public CUIObject
{
private:
	CUI_PosMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PosMark(const CUI_PosMark& Prototype);
	virtual ~CUI_PosMark() = default;

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
	void PlayerPositionCompare();

public:
	static CUI_PosMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END