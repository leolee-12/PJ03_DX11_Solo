#include "stdafx.h"

#include "UI_CharaSelectFude.h"
#include "RenderInstance.h"

CUI_CharaSelectFude::CUI_CharaSelectFude(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_CharaSelectFude::CUI_CharaSelectFude(const CUI_CharaSelectFude& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_CharaSelectFude::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectFude::Initialize(void* pArg)
{
	m_fPosX = 437.f, m_fPosY = 146.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* Desc = static_cast<UI_DESC*>(pArg);
	m_iTextureIndex  = Desc->iNumUI;

	m_fSizeX = 270.f, m_fSizeY = 135.f;
	m_fPosX = 635.f, m_fPosY = 200.f;

	InitPosition();

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_CharaSelectFude::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_CharaSelectFude::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_CharaSelectFude::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_CharaSelectFude::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(34)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectFude::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectFude"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_CharaSelectFude::InitPosition()
{
	switch (m_iTextureIndex)
	{
	case 0:
		m_fSizeX = 838.f, m_fSizeY = 838.f;
		m_fPosX = 418.f, m_fPosY = 306.f;
		break;

	case 1:
		m_fSizeX = 855.f, m_fSizeY = 855.f;
		m_fPosX = 855.f, m_fPosY = 319.f;
		break;

	default:
		break;
	}
}

CUI_CharaSelectFude* CUI_CharaSelectFude::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharaSelectFude* pInstatnce = new CUI_CharaSelectFude(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_CharaSelectFude"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_CharaSelectFude::Clone(void* pArg)
{
	CUI_CharaSelectFude* pInstatnce = new CUI_CharaSelectFude(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_CharaSelectFude"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_CharaSelectFude::Free()
{
	__super::Free();
}
