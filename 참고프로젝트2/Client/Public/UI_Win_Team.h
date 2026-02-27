#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Win_Team final :public CUIObject
{
private:
	CUI_Win_Team(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Win_Team(const CUI_Win_Team& Prototype);
	virtual ~CUI_Win_Team() = default;

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
	_float DestroyTimer = { 0.f };
	_bool m_bFinish = { FALSE };


public:
	static CUI_Win_Team* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END