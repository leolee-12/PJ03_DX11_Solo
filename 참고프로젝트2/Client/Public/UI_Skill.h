#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Skill abstract : public CUIObject
{
protected:
	CUI_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Skill(const CUI_Skill& Prototype);
	virtual ~CUI_Skill() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Ready_Components();

protected:
	_uint m_iSkillPoint = { 0 };
	_uint m_iSkillNumber = { 0 };

	_int m_iMaxSkillPoint = { 0 };

	_float m_fSkillRadio = { 0.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

