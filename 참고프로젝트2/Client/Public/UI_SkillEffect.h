#pragma once

#include "UI_Skill.h"

BEGIN(Client)

class CUI_SkillEffect final :public CUI_Skill
{
private:
	CUI_SkillEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillEffect(const CUI_SkillEffect& Prototype);
	virtual ~CUI_SkillEffect() = default;

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
	_uint m_iCurrIndex = { 0};
	_uint m_iTotalIndex = { 0 };
	_float m_fAnimTimer = { 0.f };
	_float m_fRenderTimer = { 0.f };

public:
	static CUI_SkillEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END