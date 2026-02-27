#include "stdafx.h"

#include "UI_VS_Bar.h"
#include "RenderInstance.h"

CUI_VS_Bar::CUI_VS_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_Bar::CUI_VS_Bar(const CUI_VS_Bar& Prototype)
	:CUIObject	{ Prototype }
{
}

HRESULT CUI_VS_Bar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_Bar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = 570.f, m_fPosY = 90.f;
	
	m_fSizeX = 1440.f, m_fSizeY = 180.f;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	iNumUI = pDesc->iNumUI;

	if (iNumUI == 0)
		m_fPosY = 50.f;
	else if (iNumUI == 1)
		m_fPosY = m_vPrevWinSize.y - 40.f;

	__super::Set_UI_Setting(m_fSizeX, -m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_Bar::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_Bar::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_VS_Bar::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_Bar::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iNumUI)))
		return E_FAIL;

	if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;


	_float fAlphaValue = { 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &fAlphaValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(30)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_Bar::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_MaskTexture*/
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_CharaPanelPlate"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexture))))
		return E_FAIL;


	return S_OK;
}

CUI_VS_Bar* CUI_VS_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_Bar* pInstatnce = new CUI_VS_Bar(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_Bar"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_Bar::Clone(void* pArg)
{
	CUI_VS_Bar* pInstatnce = new CUI_VS_Bar(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_Bar"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_Bar::Free()
{
	Safe_Release(m_pMaskTexture);

	__super::Free();
}
