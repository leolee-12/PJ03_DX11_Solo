#include "stdafx.h"

#include "UI_VS_StaticLight.h"
#include "RenderInstance.h"

CUI_VS_StaticLight::CUI_VS_StaticLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_StaticLight::CUI_VS_StaticLight(const CUI_VS_StaticLight& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_StaticLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_StaticLight::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = m_vPrevWinSize.x * 0.5f, m_fPosY = m_vPrevWinSize.y * 0.5f;
	m_fSizeX = 200.f, m_fSizeY = 200.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_StaticLight::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_StaticLight::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CUI_VS_StaticLight::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 3.2f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CUI_VS_StaticLight::Render(_float fTimeDelta)
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

HRESULT CUI_VS_StaticLight::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_StaticLight"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_VS_StaticLight* CUI_VS_StaticLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_StaticLight* pInstatnce = new CUI_VS_StaticLight(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_StaticLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_StaticLight::Clone(void* pArg)
{
	CUI_VS_StaticLight* pInstatnce = new CUI_VS_StaticLight(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_StaticLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_StaticLight::Free()
{
	__super::Free();
}
