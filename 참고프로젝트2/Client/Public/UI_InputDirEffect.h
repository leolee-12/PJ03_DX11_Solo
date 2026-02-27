#pragma once

#include "UI_Input.h"

BEGIN(Client)

class CUI_InputDirEffect final :public CUI_Input
{
public:
	typedef struct :public CUIObject::UI_DESC
	{
		_vector vCreatePos = {};
		_float fAngle = { };
		_float fScaled = { 0 };
	}UI_DIREFFECT;

private:
	CUI_InputDirEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InputDirEffect(const CUI_InputDirEffect& Prototype);
	virtual ~CUI_InputDirEffect() = default;

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
	_float m_fDestroyTimer = { 0.f };

public:
	static CUI_InputDirEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END