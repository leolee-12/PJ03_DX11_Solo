#include "stdafx.h"

#include "UI_LoadingSpaceLight.h"
#include "RenderInstance.h"

#include "UI_Loading_FlyEff.h"

CUI_LoadingSpaceLight::CUI_LoadingSpaceLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_LoadingSpaceLight::CUI_LoadingSpaceLight(const CUI_LoadingSpaceLight& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_LoadingSpaceLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingSpaceLight::Initialize(void* pArg)
{
	m_fPosX = 731, m_fPosY = 166;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 100.f, m_fSizeY = 30.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_LoadingSpaceLight::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_LoadingSpaceLight::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	_bool bEmpty = m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")).empty();

	//if (bEmpty == FALSE)
	//{
	//	_bool bAnimEnd = dynamic_cast<CUI_Loading_FlyEff*>(m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")).back())->Get_AnimEnd();
	//
	//	if (bAnimEnd && m_pUI_Manager->m_iNumThreadFinish >= 7)
	//	{
	//		Destory();
	//	}
	//}
}

void CUI_LoadingSpaceLight::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);


	AlphaSwiching(m_fAlphaValue, fTimeDelta);
	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 7;
	tDesc.tGlowDesc.fGlowFactor = 6.3f;

	//__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this ,&tDesc);
}

HRESULT CUI_LoadingSpaceLight::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(20)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingSpaceLight::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingSpaceLight"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_LoadingSpaceLight::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_LoadingSpaceLight::AlphaSwiching(_float& fAlphaValue , _float fTimeDelta)
{
	m_IsAlphaSwich ? (fAlphaValue += fTimeDelta * 0.5f) : (fAlphaValue -= fTimeDelta * 0.5f);

	if (fAlphaValue <= 0.5f)
		m_IsAlphaSwich = TRUE;
	else if (fAlphaValue >= 1.f)
		m_IsAlphaSwich = FALSE;
}

void CUI_LoadingSpaceLight::ScaleAnim()
{
	//m_fAlphaValue
}

CUI_LoadingSpaceLight* CUI_LoadingSpaceLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingSpaceLight* pInstatnce = new CUI_LoadingSpaceLight(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_LoadingSpaceLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_LoadingSpaceLight::Clone(void* pArg)
{
	CUI_LoadingSpaceLight* pInstatnce = new CUI_LoadingSpaceLight(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_LoadingSpaceLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_LoadingSpaceLight::Free()
{
	__super::Free();
}
