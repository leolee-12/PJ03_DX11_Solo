#pragma once

#include "UI_Input.h"

BEGIN(Client)

class CUI_Input_DirPanel final :public CUI_Input
{
private:
	CUI_Input_DirPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Input_DirPanel(const CUI_Input_DirPanel& Prototype);
	virtual ~CUI_Input_DirPanel() = default;

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
	static CUI_Input_DirPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END