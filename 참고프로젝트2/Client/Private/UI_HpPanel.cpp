
#include "stdafx.h"

#include "UI_HpPanel.h"
#include "RenderInstance.h"
#include "RenderInstance.h"

CUI_HpPanel::CUI_HpPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_HpPanel::CUI_HpPanel(const CUI_HpPanel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_HpPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HpPanel::Initialize(void* pArg)
{
	m_fSizeX = 464.f, m_fSizeY = 116.f, m_fPosX = 330.f, m_fPosY = 67.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX , m_fSizeY, m_fPosX , m_fPosY,0.8f);


	return S_OK;
}

void CUI_HpPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_HpPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Animation({ 271 ,107 ,0.8, 1.f }, { m_fPosX, m_fPosY, 0.8f, 1.f }, 500.f, 0.8f, fTimeDelta);

}

void CUI_HpPanel::Late_Update(_float fTimeDelta)
{

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_HpPanel::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

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

HRESULT CUI_HpPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_HpPanel* CUI_HpPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HpPanel* pInstatnce = new CUI_HpPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_HpPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_HpPanel::Clone(void* pArg)
{
	CUI_HpPanel* pInstatnce = new CUI_HpPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_HpPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_HpPanel::Free()
{
	__super::Free();
}
