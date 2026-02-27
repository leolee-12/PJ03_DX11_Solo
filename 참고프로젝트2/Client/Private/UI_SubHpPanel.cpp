#include "stdafx.h"

#include "UI_SubHpPanel.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_SubHpPanel::CUI_SubHpPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_SubHpPanel::CUI_SubHpPanel(const CUI_SubHpPanel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_SubHpPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SubHpPanel::Initialize(void* pArg)
{
	m_fPosX = 240.f;
	m_fSizeX = 213.f;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosY = 103.f;
	__super::Set_UI_Setting(m_fSizeX , 71.f, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_SubHpPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_SubHpPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	Animation({ 255.f , 67.f ,0.7f, 1.f }, { m_fPosX, m_fPosY, 0.7f, 1.f }, 500.f, 0.7f, fTimeDelta);
}

void CUI_SubHpPanel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_pSubPawn != nullptr)
	{
		if (m_pSubPawn->Get_PawnDesc().iHp > 0.f)
			m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
	}
}

HRESULT CUI_SubHpPanel::Render(_float fTimeDelta)
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

HRESULT CUI_SubHpPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SubHpPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_SubHpPanel* CUI_SubHpPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SubHpPanel* pInstatnce = new CUI_SubHpPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SubHpPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SubHpPanel::Clone(void* pArg)
{
	CUI_SubHpPanel* pInstatnce = new CUI_SubHpPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SubHpPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SubHpPanel::Free()
{
	__super::Free();
}
