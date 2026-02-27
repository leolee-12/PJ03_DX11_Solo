#include "stdafx.h"

#include "UI_Skill.h"
#include "RenderInstance.h"
#include "Character.h"
#include "BattleInterface.h"

CUI_Skill::CUI_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice , pContext }
{

}

CUI_Skill::CUI_Skill(const CUI_Skill& Prototype)
	:CUIObject{ Prototype }
{

}

HRESULT CUI_Skill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Skill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iMaxSkillPoint = 100;

	return S_OK;
}

void CUI_Skill::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	//m_iSkillPoint = max(m_pMainPawn->Get_PawnDesc().iSKillPoint , m_pSubPawn->Get_PawnDesc().iSKillPoint);
	
	if (m_pMainPawn != nullptr)
	{
		if (m_eLRPos == LEFT)
		{
			
			m_iSkillPoint = CBattleInterface_Manager::Get_Instance()->Get_KiGuage(1);
			m_iSkillNumber = CBattleInterface_Manager::Get_Instance()->Get_KiNumber(1);
		}
		else if (m_eLRPos == RIGHT)
		{
			m_iSkillPoint = CBattleInterface_Manager::Get_Instance()->Get_KiGuage(2);
			m_iSkillNumber = CBattleInterface_Manager::Get_Instance()->Get_KiNumber(2);
		}
	
	}

	m_fSkillRadio = 1 - m_iSkillPoint / 100.f;

	if (m_iSkillNumber >= 7)
		m_fSkillRadio = 1.f;
}

void CUI_Skill::Update(_float fTimeDelta)
{
	
}

void CUI_Skill::Late_Update(_float fTimeDelta)
{

}

HRESULT CUI_Skill::Render(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Skill::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Skill::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Skill::Free()
{

	__super::Free();
}
