#include "stdafx.h"

#include "UI_VS_Mark.h"
#include "RenderInstance.h"

CUI_VS_Mark::CUI_VS_Mark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_VS_Mark::CUI_VS_Mark(const CUI_VS_Mark& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_VS_Mark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_Mark::Initialize(void* pArg)
{
	m_fSizeX = 800.f, m_fSizeY = 1600.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg); 
	m_iTextureIndex = pDesc->iNumUI;

	if(m_iTextureIndex == 0)
		m_fPosX = 630.f, m_fPosY = 305.f;
	else if (m_iTextureIndex == 1)
		m_fPosX = 630.f, m_fPosY = 320.f;

	if (m_iTextureIndex == 0)
	{
		Set_AnimPosition(400.f, 0.25f);
		Set_AnimPosition(400.f, 0.5f);
		Set_AnimPosition(200.f, 0.75f);
		Set_AnimPosition(200.f, 0.85f);
		Set_AnimPosition(400.f, 1.f);

		m_fAlphaValue = 0.5f;
	}
	else if (m_iTextureIndex == 1)
	{
		Set_AnimPosition(800.f, 0.1f);
		Set_AnimPosition(400.f, 0.5f);
		Set_AnimPosition(200.f, 0.75f);
		Set_AnimPosition(200.f, 0.85f);
		Set_AnimPosition(400.f, 1.f);

		m_fAlphaValue = 0.f;
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_Mark::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_Mark::Update(_float fTimeDelta)
{
	if (fTimeDelta >= 0.1f)
		fTimeDelta = 0.f;

	__super::Update(fTimeDelta);

	if(m_iTextureIndex == 0)
		m_pUI_Manager->m_fTotalDuration += fTimeDelta;

	m_fAlphaValue += fTimeDelta;

	if (m_fAlphaValue >= 1.f)
		m_fAlphaValue = 1.f;

	if(m_QueueAnim.empty())
		m_pUI_Manager->m_fTotalDuration = 0.f;

	Action_Anim(2.f, fTimeDelta);
}

void CUI_VS_Mark::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI , this);
}

HRESULT CUI_VS_Mark::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_Mark::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_Mark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_VS_Mark* CUI_VS_Mark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_Mark* pInstatnce = new CUI_VS_Mark(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_Mark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_Mark::Clone(void* pArg)
{
	CUI_VS_Mark* pInstatnce = new CUI_VS_Mark(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_Mark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_Mark::Free()
{
	__super::Free();
}
