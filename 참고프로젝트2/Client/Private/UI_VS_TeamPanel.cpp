#include "stdafx.h"

#include "UI_VS_TeamPanel.h"
#include "RenderInstance.h"

CUI_VS_TeamPanel::CUI_VS_TeamPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_TeamPanel::CUI_VS_TeamPanel(const CUI_VS_TeamPanel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_TeamPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_TeamPanel::Initialize(void* pArg)
{
	m_fPosX = 1050.f, m_fPosY = 580.f;
	m_fSizeX = 675.f , m_fSizeY = 335.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_TeamPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_TeamPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_VS_TeamPanel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_TeamPanel::Render(_float fTimeDelta)
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

HRESULT CUI_VS_TeamPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_TeamPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_VS_TeamPanel* CUI_VS_TeamPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_TeamPanel* pInstatnce = new CUI_VS_TeamPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_TeamPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_TeamPanel::Clone(void* pArg)
{
	CUI_VS_TeamPanel* pInstatnce = new CUI_VS_TeamPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_TeamPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_TeamPanel::Free()
{
	__super::Free();
}
