#include "stdafx.h"
#include "..\Public\UI_Chara_Select_BG.h"

#include "GameInstance.h"
#include "RenderInstance.h"

#include "UI_Define.h"

CUI_Chara_Select_BG::CUI_Chara_Select_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUI_Chara_Select_BG::CUI_Chara_Select_BG(const CUI_Chara_Select_BG& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CUI_Chara_Select_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Chara_Select_BG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fPosX = g_iWinSizeX >> 1;
	m_fPosY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fPosX - g_iWinSizeX * 0.5f, -m_fPosY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Chara_Select_BG::Camera_Update(_float fTimeDelta)
{
	
}

void CUI_Chara_Select_BG::Update(_float fTimeDelta)
{
	m_fMarkTimer += fTimeDelta* 0.025f;
	if (m_fMarkTimer >= 1.f)
		m_fMarkTimer = 0.f;

}

void CUI_Chara_Select_BG::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_Chara_Select_BG::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(24)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CUI_Chara_Select_BG::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharaSelect_BG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectLineFrame"),
		TEXT("Com_BGTexture"), reinterpret_cast<CComponent**>(&m_pBGTexture))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectDustEffect"),
		TEXT("Com_DustTexture"), reinterpret_cast<CComponent**>(&m_pDustTexutre))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Chara_Select_BG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pBGTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pDustTexutre->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fMarkTimer, sizeof(_float))))
		return E_FAIL;

	


	return S_OK;
}

CUI_Chara_Select_BG* CUI_Chara_Select_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chara_Select_BG* pInstance = new CUI_Chara_Select_BG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Chara_Select_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Chara_Select_BG::Clone(void* pArg)
{
	CUI_Chara_Select_BG* pInstance = new CUI_Chara_Select_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Chara_Select_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Chara_Select_BG::Free()
{
	Safe_Release(m_pBGTexture);
	Safe_Release(m_pDustTexutre);

	__super::Free();
}
