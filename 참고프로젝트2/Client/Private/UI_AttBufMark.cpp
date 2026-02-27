#include "stdafx.h"

#include "UI_AttBufMark.h"
#include "RenderInstance.h"

CUI_AttBufMark::CUI_AttBufMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_BaseAttBuf{ pDevice ,pContext }
{
}

CUI_AttBufMark::CUI_AttBufMark(const CUI_AttBufMark& Prototype)
	:CUI_BaseAttBuf{ Prototype }
{
}

HRESULT CUI_AttBufMark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttBufMark::Initialize(void* pArg)
{
	m_fPosX = 510.f, m_fPosY = 112.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 40.f, m_fSizeY = 40.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY,0.f);

	return S_OK;
}

void CUI_AttBufMark::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_AttBufMark::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CUI_AttBufMark::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_AttBufMark::Render(_float fTimeDelta)
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

HRESULT CUI_AttBufMark::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufMark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_AttBufMark* CUI_AttBufMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AttBufMark* pInstatnce = new CUI_AttBufMark(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_AttBufMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_AttBufMark::Clone(void* pArg)
{
	CUI_AttBufMark* pInstatnce = new CUI_AttBufMark(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_AttBufMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_AttBufMark::Free()
{
	__super::Free();
}
