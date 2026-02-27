#pragma once

#include "UI_Input.h"

BEGIN(Client)

class CUI_Input_ActionIcon final :public CUI_Input
{


private:
	CUI_Input_ActionIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Input_ActionIcon(const CUI_Input_ActionIcon& Prototype);
	virtual ~CUI_Input_ActionIcon() = default;

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
	void InputGrab(_uint iTexIndex);
	void InputGrabPosition(_float& fPos);

private:
	_uint m_iNumCommandList = { 0 };
	_uint m_iTextureIndex = { 0 };


public:
	static CUI_Input_ActionIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END