#include "stdafx.h"

#include "UI_AttBuf.h"
#include "RenderInstance.h"

CUI_AttBuf::CUI_AttBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_BaseAttBuf{ pDevice ,pContext }
{
}

CUI_AttBuf::CUI_AttBuf(const CUI_AttBuf& Prototype)
	:CUI_BaseAttBuf{ Prototype }
{
}

HRESULT CUI_AttBuf::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttBuf::Initialize(void* pArg)
{
	m_fSizeX = 40.f, m_fSizeY = 40.f, m_fPosX = 510.f, m_fPosY = 112.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.1f);

	return S_OK;
}

void CUI_AttBuf::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fAnimFrame += 6.0f * fTimeDelta;

	if (m_fAnimFrame >= 3.f)
		m_fAnimFrame = 0.f;
}

void CUI_AttBuf::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_AttBuf::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_AttBuf::Render(_float fTimeDelta)
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

HRESULT CUI_AttBuf::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBuf"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_AttBuf* CUI_AttBuf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AttBuf* pInstatnce = new CUI_AttBuf(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_AttBuf"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_AttBuf::Clone(void* pArg)
{
	CUI_AttBuf* pInstatnce = new CUI_AttBuf(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_AttBuf"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_AttBuf::Free()
{
	__super::Free();
}
