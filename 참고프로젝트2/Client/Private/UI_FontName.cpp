#include "stdafx.h"

#include "UI_FontName.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_FontName::CUI_FontName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_FontName::CUI_FontName(const CUI_FontName& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_FontName::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FontName::Initialize(void* pArg)
{
	m_fPosX = 245;

	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (m_pMainPawn != nullptr)
		m_iTexIdx = m_pMainPawn->Get_PawnDesc().ePlayerID;

	if (m_iTexIdx == CUI_Define::HIT)
		m_fPosX -= 30;


	__super::Set_UI_Setting(255.f, 32.f, m_fPosX, 27, 0.f);


	return S_OK;
}

void CUI_FontName::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	if (m_pMainPawn != nullptr)
		m_iTexIdx = m_pMainPawn->Get_PawnDesc().ePlayerID;

}

void CUI_FontName::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_FontName::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_FontName::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FontName::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_FontNameGamePlay"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_FontName* CUI_FontName::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_FontName* pInstatnce = new CUI_FontName(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_FontName"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_FontName::Clone(void* pArg)
{
	CUI_FontName* pInstatnce = new CUI_FontName(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_FontName"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_FontName::Free()
{
	__super::Free();
}
