#include "stdafx.h"
#include "..\Public\UI_Logo_BG.h"

#include "GameInstance.h"
#include "RenderInstance.h"

#include "UI_Define.h"

CUI_Logo_BG::CUI_Logo_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUI_Logo_BG::CUI_Logo_BG(const CUI_Logo_BG& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CUI_Logo_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Logo_BG::Initialize(void* pArg)
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
		XMVectorSet(m_fPosX - g_iWinSizeX * 0.5f, -m_fPosY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Logo_BG::Camera_Update(_float fTimeDelta)
{
	m_fVideoSprite += fTimeDelta * 12.f;

	if (m_fVideoSprite >= 145)
		m_fVideoSprite == 0;
}

void CUI_Logo_BG::Update(_float fTimeDelta)
{
}

void CUI_Logo_BG::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_Logo_BG::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(19)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CUI_Logo_BG::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VideoTexture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_LogoVideoTex"),
		TEXT("Com_VideoTexture"), reinterpret_cast<CComponent**>(&m_pVideoTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Logo_BG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pVideoTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", m_fVideoSprite)))
		return E_FAIL;


	return S_OK;
}

CUI_Logo_BG* CUI_Logo_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Logo_BG* pInstance = new CUI_Logo_BG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Logo_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Logo_BG::Clone(void* pArg)
{
	CUI_Logo_BG* pInstance = new CUI_Logo_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Logo_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Logo_BG::Free()
{
	Safe_Release(m_pVideoTextureCom);

	__super::Free();
}
