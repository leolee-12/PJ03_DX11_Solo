#include "stdafx.h"

#include "UI_AttBufNone.h"
#include "RenderInstance.h"

CUI_AttBufNone::CUI_AttBufNone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_BaseAttBuf{ pDevice ,pContext }
{
}

CUI_AttBufNone::CUI_AttBufNone(const CUI_AttBufNone& Prototype)
	:CUI_BaseAttBuf{ Prototype }
{
}

HRESULT CUI_AttBufNone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttBufNone::Initialize(void* pArg)
{
	m_fSizeX = 40.f, m_fSizeY = 40.f, m_fPosX = 510.f, m_fPosY = 112.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 1.f);

	return S_OK;
}

void CUI_AttBufNone::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_AttBufNone::Update(_float fTimeDelta)
{

}

void CUI_AttBufNone::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_AttBufNone::Render(_float fTimeDelta)
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

HRESULT CUI_AttBufNone::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufNone"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_AttBufNone* CUI_AttBufNone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AttBufNone* pInstatnce = new CUI_AttBufNone(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_AttBufNone"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_AttBufNone::Clone(void* pArg)
{
	CUI_AttBufNone* pInstatnce = new CUI_AttBufNone(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_AttBufNone"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_AttBufNone::Free()
{
	__super::Free();
}
