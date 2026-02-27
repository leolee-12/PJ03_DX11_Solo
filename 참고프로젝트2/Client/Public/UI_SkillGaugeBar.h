#pragma once

#include "UI_Skill.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CUI_SkillGaugeBar final :public CUI_Skill
{
private:
	CUI_SkillGaugeBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillGaugeBar(const CUI_SkillGaugeBar& Prototype);
	virtual ~CUI_SkillGaugeBar() = default;

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
	_float m_fMaskUVTimer = { 0.f };

	CTexture* m_pSkillTexture[2] = {nullptr,  nullptr };
	CTexture* m_pSkillFlowEffect = { nullptr };


public:
	static CUI_SkillGaugeBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END