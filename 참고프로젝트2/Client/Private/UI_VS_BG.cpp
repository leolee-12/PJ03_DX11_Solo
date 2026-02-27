#include "stdafx.h"
#include "..\Public\UI_VS_BG.h"

#include "GameInstance.h"
#include "RenderInstance.h"

#include "UI_Define.h"

CUI_VS_BG::CUI_VS_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUI_VS_BG::CUI_VS_BG(const CUI_VS_BG& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CUI_VS_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_VS_BG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX *1.75f;
	m_fSizeY = g_iWinSizeY *1.75f;
	m_fPosX = g_iWinSizeX >> 1;
	m_fPosY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fPosX - g_iWinSizeX * 0.5f, -m_fPosY + g_iWinSizeY * 0.5f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_VS_BG::Camera_Update(_float fTimeDelta)
{

}

void CUI_VS_BG::Update(_float fTimeDelta)
{
	m_fMaskMoveValue += fTimeDelta * 90.f;

	if (m_fMaskMoveValue >= 360.f)
		m_fMaskMoveValue = 0.f;
}

void CUI_VS_BG::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_VS_BG::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(27)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_BG::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_TextureColor */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Color"),
		TEXT("Com_TextureColor"), reinterpret_cast<CComponent**>(&m_pTextureColor))))
		return E_FAIL;
	
	/* Com_TextureLine */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Line"),
		TEXT("Com_TextureLine"), reinterpret_cast<CComponent**>(&m_pTextureLine))))
		return E_FAIL; 

	//g_MaskTimer

	//MarkTextrue
	/* Com_TextureMark */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle0"),
		TEXT("Com_TextureMark"), reinterpret_cast<CComponent**>(&m_pTextureCircle[0]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle1"),
		TEXT("Com_TextureMark1"), reinterpret_cast<CComponent**>(&m_pTextureCircle[1]))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle2"),
		TEXT("Com_TextureMark2"), reinterpret_cast<CComponent**>(&m_pTextureCircle[2]))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle3"),
		TEXT("Com_TextureMark3"), reinterpret_cast<CComponent**>(&m_pTextureCircle[3]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_BG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureColor->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureLine->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	
	if (FAILED(m_pTextureCircle[0]->Bind_ShaderResource(m_pShaderCom, "g_CircleTexture0", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCircle[1]->Bind_ShaderResource(m_pShaderCom, "g_CircleTexture1", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCircle[2]->Bind_ShaderResource(m_pShaderCom, "g_CircleTexture2", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCircle[3]->Bind_ShaderResource(m_pShaderCom, "g_CircleTexture3", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fMaskMoveValue, sizeof(_float))))
		return E_FAIL;
	

	return S_OK;
}

CUI_VS_BG* CUI_VS_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_BG* pInstance = new CUI_VS_BG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_VS_BG::Clone(void* pArg)
{
	CUI_VS_BG* pInstance = new CUI_VS_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_VS_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_VS_BG::Free()
{
	Safe_Release(m_pTextureLine);
	Safe_Release(m_pTextureColor);

	for(int i = 0 ; i < 4; i++)
		Safe_Release(m_pTextureCircle[i]);

	__super::Free();
}
