#include "stdafx.h"

#include "UI_Loading_CreateFlyEff.h"
#include "RenderInstance.h"

CUI_Loading_CreateFlyEff::CUI_Loading_CreateFlyEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Loading_CreateFlyEff::CUI_Loading_CreateFlyEff(const CUI_Loading_CreateFlyEff& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Loading_CreateFlyEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_CreateFlyEff::Initialize(void* pArg)
{
	m_fPosX = 731, m_fPosY = 166;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 50.f, m_fSizeY = 50.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Loading_CreateFlyEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Loading_CreateFlyEff::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	ScaleAnim(fTimeDelta);

	m_fAlphaValue -= fTimeDelta * 5.f;
}

void CUI_Loading_CreateFlyEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 2.3f;

	//__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this, &tDesc);
}

HRESULT CUI_Loading_CreateFlyEff::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_CreateFlyEff::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingCreateFlyEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Loading_CreateFlyEff::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_Loading_CreateFlyEff::ScaleAnim(_float fTimeDelta)
{
	m_fScaleValue += fTimeDelta * 10.f;

	if (m_fScaleValue >= 5.f)
		Destory();

	m_pTransformCom->Set_Scaled(m_fSizeX * m_fScaleValue, m_fSizeY * m_fScaleValue, 1.f);
}

CUI_Loading_CreateFlyEff* CUI_Loading_CreateFlyEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_CreateFlyEff* pInstatnce = new CUI_Loading_CreateFlyEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Loading_CreateFlyEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Loading_CreateFlyEff::Clone(void* pArg)
{
	CUI_Loading_CreateFlyEff* pInstatnce = new CUI_Loading_CreateFlyEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Loading_CreateFlyEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Loading_CreateFlyEff::Free()
{
	__super::Free();
}
