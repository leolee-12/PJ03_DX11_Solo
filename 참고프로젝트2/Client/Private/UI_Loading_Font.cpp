#include "stdafx.h"

#include "UI_Loading_Font.h"
#include "RenderInstance.h"

CUI_Loading_Font::CUI_Loading_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Loading_Font::CUI_Loading_Font(const CUI_Loading_Font& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Loading_Font::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_Font::Initialize(void* pArg)
{
	m_fSizeX = 200.f;
	m_fSizeY = 50.f;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = m_vPrevWinSize.x - 100;
	m_fPosY = m_vPrevWinSize.y - 100;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Loading_Font::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Loading_Font::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Loading_Font::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Loading_Font::Render(_float fTimeDelta)
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

HRESULT CUI_Loading_Font::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingFont"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Loading_Font* CUI_Loading_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_Font* pInstatnce = new CUI_Loading_Font(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Loading_Font"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Loading_Font::Clone(void* pArg)
{
	CUI_Loading_Font* pInstatnce = new CUI_Loading_Font(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Loading_Font"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Loading_Font::Free()
{
	__super::Free();
}
