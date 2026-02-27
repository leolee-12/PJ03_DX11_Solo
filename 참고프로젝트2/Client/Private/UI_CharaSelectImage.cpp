#include "stdafx.h"

#include "UI_CharaSelectImage.h"
#include "RenderInstance.h"

#include "UI_SelectArrow.h"

CUI_CharaSelectImage::CUI_CharaSelectImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_CharaSelectImage::CUI_CharaSelectImage(const CUI_CharaSelectImage& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_CharaSelectImage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectImage::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	UI_IMAGE_DESC* Desc = static_cast<UI_IMAGE_DESC*>(pArg);
	//m_iTexIndex = Desc->iTextureIndex;
	m_iNumChoice = Desc->iNumChoice;

	InitPosition();

	__super::Set_UI_Setting(460, 460, m_fPosX, 190, 0.0f);

	return S_OK;
}

void CUI_CharaSelectImage::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_CharaSelectImage::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_int iThieSize = m_pGameInstance->Get_Layer(LEVEL_CHARACTER, TEXT("Layer_BackGroundImage")).size() - 1;

	if(m_iNumChoice == iThieSize && !m_pGameInstance->Get_Layer(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")).empty())
		m_iTexIndex = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->GetPlayerID();
	
	

}

void CUI_CharaSelectImage::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_CharaSelectImage::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIndex)))
		return E_FAIL;

	if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(23)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectImage::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharaSelect_Image"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterBGMask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexture))))
		return E_FAIL;


	return S_OK;
}

void CUI_CharaSelectImage::InitPosition()
{
	switch (m_iNumChoice)
	{
	case 0:
		m_fPosX = 390;
		break;

	case 1:
		m_fPosX = 250;
		break;

	case 2:
		m_fPosX = 1280 -  390;
		break;

	case 3:
		m_fPosX = 1280 - 250;
		break;

	default:
		Destory();
		break;

	}
}

CUI_CharaSelectImage* CUI_CharaSelectImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharaSelectImage* pInstatnce = new CUI_CharaSelectImage(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_CharaSelectImage"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_CharaSelectImage::Clone(void* pArg)
{
	CUI_CharaSelectImage* pInstatnce = new CUI_CharaSelectImage(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_CharaSelectImage"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_CharaSelectImage::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTexture);
}
