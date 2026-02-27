#pragma once

#include "UI_Skill.h"

BEGIN(Client)

class CUI_SkillNumber final :public CUI_Skill
{
private:
	CUI_SkillNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillNumber(const CUI_SkillNumber& Prototype);
	virtual ~CUI_SkillNumber() = default;

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
	_bool m_bSignSwitch = { FALSE };
	_float m_fColorValue = { 0.f };

public:
	static CUI_SkillNumber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END