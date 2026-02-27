#include "stdafx.h"

#include "UI_Timer.h"
#include "RenderInstance.h"

CUI_Timer::CUI_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Timer::CUI_Timer(const CUI_Timer& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Timer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Timer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	__super::Set_UI_Setting(100.f, 50.f, m_vPrevWinSize.x * 0.5f, 60.f, 0.8f);

	return S_OK;
}

void CUI_Timer::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
		
}

void CUI_Timer::Update(_float fTimeDelta)
{

}

void CUI_Timer::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Timer::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 10)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Timer::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Timer"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Timer* CUI_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Timer* pInstatnce = new CUI_Timer(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Timer"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Timer::Clone(void* pArg)
{
	CUI_Timer* pInstatnce = new CUI_Timer(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Timer"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Timer::Free()
{
	__super::Free();
}
