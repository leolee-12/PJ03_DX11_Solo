#pragma once

#include "UIObject.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CUI_Sub_Chara_Icon_Panel final :public CUIObject
{
private:
	CUI_Sub_Chara_Icon_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Sub_Chara_Icon_Panel(const CUI_Sub_Chara_Icon_Panel& Prototype);
	virtual ~CUI_Sub_Chara_Icon_Panel() = default;

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
	_uint m_iCharaID = { 0 };

public:
	static CUI_Sub_Chara_Icon_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END