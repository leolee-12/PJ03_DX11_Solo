#include "stdafx.h"

#include "UI_CharaSelectMark.h"
#include "RenderInstance.h"

CUI_CharaSelectMark::CUI_CharaSelectMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_CharaSelectMark::CUI_CharaSelectMark(const CUI_CharaSelectMark& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_CharaSelectMark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectMark::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 175, m_fSizeY = 175;
	m_fPosX = 630 , m_fPosY = 160;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_CharaSelectMark::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_CharaSelectMark::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_CharaSelectMark::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_CharaSelectMark::Render(_float fTimeDelta)
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

HRESULT CUI_CharaSelectMark::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectMark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_CharaSelectMark* CUI_CharaSelectMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharaSelectMark* pInstatnce = new CUI_CharaSelectMark(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_CharaSelectMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_CharaSelectMark::Clone(void* pArg)
{
	CUI_CharaSelectMark* pInstatnce = new CUI_CharaSelectMark(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_CharaSelectMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_CharaSelectMark::Free()
{
	__super::Free();
}
