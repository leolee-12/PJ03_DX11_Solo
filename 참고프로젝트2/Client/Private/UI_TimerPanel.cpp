#include "stdafx.h"

#include "UI_TimerPanel.h"
#include "RenderInstance.h"

CUI_TimerPanel::CUI_TimerPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_TimerPanel::CUI_TimerPanel(const CUI_TimerPanel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_TimerPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_TimerPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(200.f, 200.f, m_vPrevWinSize.x * 0.5f, 60.f, 0.9f);

	return S_OK;
}

void CUI_TimerPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_TimerPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_TimerPanel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_TimerPanel::Render(_float fTimeDelta)
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

HRESULT CUI_TimerPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_TimerPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_TimerPanel* CUI_TimerPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TimerPanel* pInstatnce = new CUI_TimerPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_TimerPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_TimerPanel::Clone(void* pArg)
{
	CUI_TimerPanel* pInstatnce = new CUI_TimerPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_TimerPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_TimerPanel::Free()
{
	__super::Free();
}
