#include "stdafx.h"

#include "UI_Option.h"
#include "RenderInstance.h"

CUI_Option::CUI_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice , pContext }
{

}

CUI_Option::CUI_Option(const CUI_Option& Prototype)
	:CUIObject{ Prototype }
{

}

HRESULT CUI_Option::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Option::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bIsActive = FALSE;

	return S_OK;
}

void CUI_Option::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Option::Update(_float fTimeDelta)
{
	InitAnimation(fTimeDelta);
}

void CUI_Option::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Option::Render(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Option::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Option::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Option::InitAnimation(_float fTimeDelta)
{
	if (m_bCreateSwitch)
	{
		if (m_bInitData == FALSE)
		{
			m_fOriginPos = m_fPosY;
			m_fPosY = m_fPosY - 25.f;
			m_bInitData = TRUE;
		}

		m_fPosY += fTimeDelta * 100.f;
		if (m_fPosY >= m_fOriginPos)
		{
			m_bCreateSwitch = FALSE;
			m_bInitData = FALSE;
			m_fPosY = m_fOriginPos;
			return;
		}
		__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY);
	}

}

void CUI_Option::Free()
{

	__super::Free();
}
