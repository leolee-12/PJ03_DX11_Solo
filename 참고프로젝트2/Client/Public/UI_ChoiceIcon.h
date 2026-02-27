#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_ChoiceIcon final :public CUIObject
{
public:
	typedef struct :public CUIObject::UI_DESC
	{
		_vector vInitPosition = {};
		_uint iTextrueIndex = { 0 };
		_uint iTeam = { 0 };
	}UI_CHOICE_DESC;

private:
	CUI_ChoiceIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ChoiceIcon(const CUI_ChoiceIcon& Prototype);
	virtual ~CUI_ChoiceIcon() = default;

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
	_uint m_iTextrueIndex = { 0 };

public:
	static CUI_ChoiceIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END