#include "stdafx.h"

#include "UI_GameState.h"
#include "RenderInstance.h"

CUI_GameState::CUI_GameState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice , pContext }
{

}

CUI_GameState::CUI_GameState(const CUI_GameState& Prototype)
	:CUIObject{ Prototype }
{

}

HRESULT CUI_GameState::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GameState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	m_fAnimPos = m_fSizeX;

	return S_OK;
}

void CUI_GameState::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

}

void CUI_GameState::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fTotalAnimDuration = m_pUI_Manager->m_fTotalDuration;
}

void CUI_GameState::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_GameState::Render(_float fTimeDelta)
{
	return S_OK;
}

void CUI_GameState::Set_AnimPosition(_int iNextPosX, _float fAnimSpeed)
{
	m_QueueAnim.push_back({ iNextPosX ,fAnimSpeed});
}

void CUI_GameState::Action_Anim(_float fSizeOffSet, _float fTimeDelta)
{
	if (m_QueueAnim.empty() == FALSE )
	{
		_float fVelocity = (m_QueueAnim.at(0).iPos - m_fAnimPos) / (m_QueueAnim.at(0).fEventFrame - m_fAnimFrame);

		if (m_QueueAnim.at(0).fEventFrame >= m_fTotalAnimDuration)
		{
			m_fSizeX +=  fVelocity* fTimeDelta;
		}
		else
		{
			m_fAnimFrame = m_QueueAnim.at(0).fEventFrame;
			m_fAnimPos = m_QueueAnim.at(0).iPos;

			m_QueueAnim.pop_front();
		}

		m_pTransformCom->Set_Scaled(m_fSizeX * m_vOffSetWinSize.x, m_fSizeX  * fSizeOffSet * m_vOffSetWinSize.y, 1.f);
	}
}

HRESULT CUI_GameState::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GameState::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_GameState::Free()
{

	__super::Free();
}
