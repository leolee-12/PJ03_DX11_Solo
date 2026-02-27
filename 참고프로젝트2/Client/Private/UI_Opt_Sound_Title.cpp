#include "stdafx.h"

#include "UI_Opt_Sound_Title.h"
#include "RenderInstance.h"

CUI_Opt_Sound_Title::CUI_Opt_Sound_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Opt_Sound{ pDevice ,pContext }
{
}

CUI_Opt_Sound_Title::CUI_Opt_Sound_Title(const CUI_Opt_Sound_Title& Prototype)
	:CUI_Opt_Sound{ Prototype }
{
}

HRESULT CUI_Opt_Sound_Title::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound_Title::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = 640.f, m_fPosY = 277.f;
	m_fSizeX = 600.f, m_fSizeY = 50.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_Opt_Sound_Title::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Opt_Sound_Title::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CUI_Opt_Sound_Title::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Opt_Sound_Title::Render(_float fTimeDelta)
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

HRESULT CUI_Opt_Sound_Title::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptTitle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}



CUI_Opt_Sound_Title* CUI_Opt_Sound_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Opt_Sound_Title* pInstatnce = new CUI_Opt_Sound_Title(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Opt_Sound_Title"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Opt_Sound_Title::Clone(void* pArg)
{
	CUI_Opt_Sound_Title* pInstatnce = new CUI_Opt_Sound_Title(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Opt_Sound_Title"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Opt_Sound_Title::Free()
{
	__super::Free();
}
