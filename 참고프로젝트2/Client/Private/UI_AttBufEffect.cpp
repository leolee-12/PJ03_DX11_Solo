#include "stdafx.h"

#include "UI_AttBufEffect.h"
#include "RenderInstance.h"

CUI_AttBufEffect::CUI_AttBufEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_BaseAttBuf{ pDevice ,pContext }
{
}

CUI_AttBufEffect::CUI_AttBufEffect(const CUI_AttBufEffect& Prototype)
	:CUI_BaseAttBuf{ Prototype }
{
}

HRESULT CUI_AttBufEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}



HRESULT CUI_AttBufEffect::Initialize(void* pArg)
{
	m_fSizeX = 65.f, m_fSizeY = 65.f, m_fPosX = 510.f, m_fPosY = 112.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.1f);

	return S_OK;
}

void CUI_AttBufEffect::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fAnimFrame += 6.0f * fTimeDelta;

	if (m_fAnimFrame >= 3.f)
		m_fAnimFrame = 0.f;
}

void CUI_AttBufEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_AttBufEffect::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);


	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_AttBufEffect::Render(_float fTimeDelta)
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

HRESULT CUI_AttBufEffect::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_AttBufEffect* CUI_AttBufEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AttBufEffect* pInstatnce = new CUI_AttBufEffect(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_AttBufEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_AttBufEffect::Clone(void* pArg)
{
	CUI_AttBufEffect* pInstatnce = new CUI_AttBufEffect(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_AttBufEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_AttBufEffect::Free()
{
	__super::Free();
}
