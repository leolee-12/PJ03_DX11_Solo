#include "stdafx.h"

#include "UI_VS_Ball.h"
#include "RenderInstance.h"

CUI_VS_Ball::CUI_VS_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_Ball::CUI_VS_Ball(const CUI_VS_Ball& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_Ball::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_Ball::Initialize(void* pArg)
{
	m_fPosX = 485.f, m_fPosY = 590.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 180.f, m_fSizeY = 180.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_Ball::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_Ball::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CUI_VS_Ball::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_Ball::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_Ball::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_DragonBall"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_VS_Ball* CUI_VS_Ball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_Ball* pInstatnce = new CUI_VS_Ball(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_Ball"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_Ball::Clone(void* pArg)
{
	CUI_VS_Ball* pInstatnce = new CUI_VS_Ball(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_Ball"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_Ball::Free()
{
	__super::Free();
}
