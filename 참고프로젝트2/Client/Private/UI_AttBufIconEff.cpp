#include "stdafx.h"

#include "UI_AttBufIconEff.h"
#include "RenderInstance.h"

CUI_AttBufIconEff::CUI_AttBufIconEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_BaseAttBuf{ pDevice ,pContext }
{
}

CUI_AttBufIconEff::CUI_AttBufIconEff(const CUI_AttBufIconEff& Prototype)
	:CUI_BaseAttBuf{ Prototype }
{
}

HRESULT CUI_AttBufIconEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttBufIconEff::Initialize(void* pArg)
{
	m_fSizeX = 100.f, m_fSizeY = 100.f, m_fPosX = 50.f, m_fPosY = 60.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.05f);

	return S_OK;
}

void CUI_AttBufIconEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fAnimFrame += 10.0f * fTimeDelta;

	if (m_fAnimFrame >= 5.f)
		m_fAnimFrame = 0.f;
}

void CUI_AttBufIconEff::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_AttBufIconEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_AttBufIconEff::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_fAnimFrame)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttBufIconEff::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufElectricEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_AttBufIconEff* CUI_AttBufIconEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AttBufIconEff* pInstatnce = new CUI_AttBufIconEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_AttBufIconEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_AttBufIconEff::Clone(void* pArg)
{
	CUI_AttBufIconEff* pInstatnce = new CUI_AttBufIconEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_AttBufIconEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_AttBufIconEff::Free()
{
	__super::Free();
}
