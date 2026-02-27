#include "stdafx.h"

#include "UI_VS_CharaPanel.h"
#include "RenderInstance.h"

#include "BattleInterface.h"

CUI_VS_CharaPanel::CUI_VS_CharaPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_CharaPanel::CUI_VS_CharaPanel(const CUI_VS_CharaPanel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_CharaPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_CharaPanel::Initialize(void* pArg)
{
	m_fSizeY = 660.f;
	m_fPosX = 200, m_fPosY = 120.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iTotalNum = pDesc->iTotalNum;
	m_iNumUI = pDesc->iNumUI;
	m_eLR = pDesc->eLRPos;

	if (m_eLR == LEFT)
		m_fSizeX = -660.f;
	else if(m_eLR == RIGHT)
		m_fSizeX = 660.f;


	InitPosition();
	m_pUI_Manager->m_bChange[0] = TRUE;
	m_pUI_Manager->m_bChange[1] = TRUE;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_CharaPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_CharaPanel::Update(_float fTimeDelta)
{
	if (fTimeDelta >= 0.1f)
		fTimeDelta = 0.f;

	__super::Update(fTimeDelta);

	if(m_iNumUI == 0)
		Animation({ 0.f, m_fPosY - 200.f  ,0.f, 1.f }, { m_fPosX, m_fPosY, 0.f, 1.f }, 2100.f, 0.f, fTimeDelta , 30.f);
	else if (m_iNumUI == 1)
		Animation({ -500, m_fPosY - 200.f  ,0.f, 1.f }, { m_fPosX, m_fPosY, 0.f, 1.f }, 2700.f, 0.f, fTimeDelta , 30.f);

}

void CUI_VS_CharaPanel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_CharaPanel::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pBGTexture->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", 0)))
		return E_FAIL;

	CUI_Define::PLAYER_ID ePlayerID = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(m_iTotalNum).ePlayerID;

	if (FAILED(m_pMarkTexture->Bind_ShaderResource(m_pShaderCom, "g_MarkTexture", ePlayerID)))
		return E_FAIL;

	if (FAILED(m_pNameTexture->Bind_ShaderResource(m_pShaderCom, "g_NameTexture", ePlayerID)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(28)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_CharaPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_CharaPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_BGTexture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_CharaPanelPlate"),
		TEXT("Com_BGTexture "), reinterpret_cast<CComponent**>(&m_pBGTexture))))
		return E_FAIL;

	/* For.Com_MarkTexture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_CharaIcon"),
		TEXT("Com_MarkTexture"), reinterpret_cast<CComponent**>(&m_pMarkTexture))))
		return E_FAIL;

	/* For.Com_NameTexture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_FontName"),
		TEXT("Com_NameTexture "), reinterpret_cast<CComponent**>(&m_pNameTexture))))
		return E_FAIL;


	return S_OK;
}

void CUI_VS_CharaPanel::InitPosition()
{
	switch (m_iNumUI)
	{
	case 0:
		m_fPosY = 150.f;
		break;

	case 1:
		m_fPosY = 400.f;
		break;

	default:
		break;
	}
}

CUI_VS_CharaPanel* CUI_VS_CharaPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_CharaPanel* pInstatnce = new CUI_VS_CharaPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_CharaPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_CharaPanel::Clone(void* pArg)
{
	CUI_VS_CharaPanel* pInstatnce = new CUI_VS_CharaPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_CharaPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_CharaPanel::Free()
{
	Safe_Release(m_pBGTexture);
	Safe_Release(m_pMarkTexture);
	Safe_Release(m_pNameTexture);

	__super::Free();
}
