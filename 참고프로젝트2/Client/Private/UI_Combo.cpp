#include "stdafx.h"

#include "UI_Combo.h"
#include "RenderInstance.h"
#include "Character.h"

#include"BattleInterface.h"

CUI_Combo::CUI_Combo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice , pContext }
{

}

CUI_Combo::CUI_Combo(const CUI_Combo& Prototype)
	:CUIObject{ Prototype }
{

}

HRESULT CUI_Combo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Combo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Combo::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);



}

void CUI_Combo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CUI_Combo::Late_Update(_float fTimeDelta)
{
	if (m_pMainPawn != nullptr)
	{
		m_bComboEnd = m_pMainPawn->Get_PawnDesc().bStun;

		if (m_pUI_Manager->m_bZeroHp)
			m_bComboEnd = FALSE;

		//if (m_pMainPawn->Get_PawnDesc().bStun || m_pUI_Manager->m_bZeroHp)
		//	m_bComboEnd = TRUE;
		//else
		//	m_bComboEnd = FALSE;
	}

	if (m_bCharaStun == FALSE)
		m_fDestoryTimer += fTimeDelta;

	if (m_bComboEnd == TRUE)
	{
		m_fAlphaTimer = 0.f;
		m_fDestoryTimer = 0.f;
		m_bCharaStun = TRUE;
	}

	if (m_bComboEnd == FALSE)
	{
		m_fAlphaTimer += fTimeDelta;
		if (m_fAlphaTimer >= 1.f)
		{
			m_fAlphaTimer = 0.f;
			m_bCharaStun = FALSE;
		}
	}

	if (m_bCharaStun == FALSE && m_fDestoryTimer >= 0.25f)
	{
		m_iComboCount = 0;
		m_fDestoryTimer = 0.f;
	}

	if (m_bComboEnd)
		m_bComboHit = TRUE;

	if (m_bComboHit && m_bCharaStun)
	{
		m_iComboCount = 0;
		m_bComboHit = FALSE;
	}

	if (m_pMainPawn != nullptr)
	{
		if (m_pMainPawn->Get_PawnDesc().iComboCount >= 2)
			m_iComboCount = m_pMainPawn->Get_PawnDesc().iComboCount;
	}

}

HRESULT CUI_Combo::Render(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Combo::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Combo::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	return S_OK;
}

_uint CUI_Combo::SetColor()
{
	if (m_iComboCount >= 50)
	{
		m_eColorVaule = PURPLE;
	}
	else if (m_iComboCount >= 10)
	{
		m_eColorVaule = BLUE;
	}
	else
		m_eColorVaule = RED;

	return m_eColorVaule;
}

void CUI_Combo::Free()
{

	__super::Free();
}
