#include "stdafx.h"
#include "..\Public\UI_Loading_BG.h"

#include "GameInstance.h"
#include "RenderInstance.h"

#include "UI_Loading_FlyEff.h"

CUI_Loading_BG::CUI_Loading_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUI_Loading_BG::CUI_Loading_BG(const CUI_Loading_BG& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CUI_Loading_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Loading_BG::Initialize(void* pArg)
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
		XMVectorSet(m_fPosX - g_iWinSizeX * 0.5f, -m_fPosY + g_iWinSizeY * 0.5f, 0.1f, 1.f));


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CUI_Loading_BG::Camera_Update(_float fTimeDelta)
{
	
}

void CUI_Loading_BG::Update(_float fTimeDelta)
{
	//_bool bEmpty =  m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")).empty();
	//_bool bAnimEnd = FALSE;
	//if (bEmpty == FALSE)
	//	bAnimEnd  = dynamic_cast<CUI_Loading_FlyEff*>(m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")).back())->Get_AnimEnd();
	//
	//if (bAnimEnd && m_pUI_Manager->m_iNumThreadFinish >= 7 && m_bBallFinish == FALSE)
	//{
	//	m_iTextureIndex = 1;
	//	m_fBGAlphaValue = 0.f;
	//	m_bBallFinish = TRUE;
	//}
	//
	//if (m_bBallFinish == FALSE)
	//{
		m_IsAlphaSwich ? (m_fBGAlphaValue += fTimeDelta * 0.25f) : (m_fBGAlphaValue -= fTimeDelta * 0.25f);

		if (m_fBGAlphaValue <= 0.25f)
			m_IsAlphaSwich = TRUE;
		else if (m_fBGAlphaValue >= 1.f)
			m_IsAlphaSwich = FALSE;
	//}
	//else
	//{
	//	m_fBGAlphaValue += fTimeDelta * 0.25f;
	//	if (m_fBGAlphaValue >= 1.f)
	//		m_fBGAlphaValue = 1.f;
	//}
}

void CUI_Loading_BG::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_Loading_BG::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(18)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_BG::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBackGround"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_TextureBG */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBackGround_Mask"),
		TEXT("Com_TextureBG"), reinterpret_cast<CComponent**>(&m_pBGTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_BG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pBGTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fBGAlphaValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_Loading_BG* CUI_Loading_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_BG* pInstance = new CUI_Loading_BG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Loading_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Loading_BG::Clone(void* pArg)
{
	CUI_Loading_BG* pInstance = new CUI_Loading_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Loading_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Loading_BG::Free()
{

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBGTextureCom);

	__super::Free();
}
