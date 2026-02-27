#include "stdafx.h"

#include "UI_AttBufThunderEffect.h"
#include "RenderInstance.h"

CUI_AttBufThunderEffect::CUI_AttBufThunderEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_BaseAttBuf{ pDevice ,pContext }
{
}

CUI_AttBufThunderEffect::CUI_AttBufThunderEffect(const CUI_AttBufThunderEffect& Prototype)
	:CUI_BaseAttBuf{ Prototype }
{
}

HRESULT CUI_AttBufThunderEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttBufThunderEffect::Initialize(void* pArg)
{
	m_fSizeX = 65.f, m_fSizeY = 65.f, m_fPosX = 510.f, m_fPosY = 112.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.05f);

	return S_OK;
}

void CUI_AttBufThunderEffect::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fAnimFrame += 10.0f * fTimeDelta;

	if (m_fAnimFrame >= 5.f)
		m_fAnimFrame = 0.f;
}

void CUI_AttBufThunderEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_AttBufThunderEffect::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_AttBufThunderEffect::Render(_float fTimeDelta)
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

HRESULT CUI_AttBufThunderEffect::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufElectricEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_AttBufThunderEffect* CUI_AttBufThunderEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AttBufThunderEffect* pInstatnce = new CUI_AttBufThunderEffect(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_AttBufThunderEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_AttBufThunderEffect::Clone(void* pArg)
{
	CUI_AttBufThunderEffect* pInstatnce = new CUI_AttBufThunderEffect(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_AttBufThunderEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_AttBufThunderEffect::Free()
{
	__super::Free();
}
