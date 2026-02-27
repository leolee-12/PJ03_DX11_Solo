#include "stdafx.h"

#include "UI_Input_IconPanel.h"
#include "RenderInstance.h"

CUI_Input_IconPanel::CUI_Input_IconPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_Input_IconPanel::CUI_Input_IconPanel(const CUI_Input_IconPanel& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_Input_IconPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_IconPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iOffsetScaled = pDesc->iNumUI;


	if(m_eLRPos == LEFT)
		m_fPosX = 20.f;
	else if (m_eLRPos == RIGHT)
		m_fPosX = m_vPrevWinSize.x - 20.f;

	
	if (m_eLRPos == LEFT)
	{
		if (m_iOffsetScaled == 2)
			m_fPosX += 20;
		else if (m_iOffsetScaled == 3)
			m_fPosX += 40;
	}
	else if (m_eLRPos == RIGHT)
	{
		if (m_iOffsetScaled == 2)
			m_fPosX -= 20;
		else if (m_iOffsetScaled == 3)
			m_fPosX -= 40;
	}

	m_fPosY = 190;
	m_fSizeX = 45.f * (m_iOffsetScaled);
	m_fSizeY = 45.f;


	if (m_eLRPos == LEFT)
		m_iNumCommandList = m_pUI_Manager->m_iNumCommandList;

	else if (m_eLRPos == RIGHT)
		m_iNumCommandList = m_pUI_Manager->m_iNumCommandList2;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_Input_IconPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Input_IconPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_float fOffSetPosY = 0.f;

	if (m_eLRPos == LEFT)
		fOffSetPosY = m_fPosY + (40 * (m_pUI_Manager->m_iNumCommandList - m_iNumCommandList));
	else if(m_eLRPos == RIGHT)
		fOffSetPosY = m_fPosY + (40 * (m_pUI_Manager->m_iNumCommandList2 - m_iNumCommandList));

	if (fOffSetPosY >= 575)
		Destory();

	__super::Set_UI_Setting(45.f * m_iOffsetScaled, 45.f, m_fPosX, fOffSetPosY, 0.9f);
}

void CUI_Input_IconPanel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Input_IconPanel::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_IconPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_InputIconPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Input_IconPanel* CUI_Input_IconPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Input_IconPanel* pInstatnce = new CUI_Input_IconPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Input_IconPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Input_IconPanel::Clone(void* pArg)
{
	CUI_Input_IconPanel* pInstatnce = new CUI_Input_IconPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Input_IconPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Input_IconPanel::Free()
{
	__super::Free();
}
