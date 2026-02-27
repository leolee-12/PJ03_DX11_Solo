#include "stdafx.h"

#include "UI_VS_DynamicLight.h"
#include "RenderInstance.h"

CUI_VS_DynamicLight::CUI_VS_DynamicLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_DynamicLight::CUI_VS_DynamicLight(const CUI_VS_DynamicLight& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_DynamicLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_DynamicLight::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = m_vPrevWinSize.x * 0.5f, m_fPosY = m_vPrevWinSize.y * 0.5f;
	m_fSizeX = 200.f, m_fSizeY = 200.f;

	_uint iRotaionValue = rand() % 360;
	m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(iRotaionValue));

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.1f);

	return S_OK;
}

void CUI_VS_DynamicLight::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_DynamicLight::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fSizeX += fTimeDelta * 2000.f;
	m_fSizeY += fTimeDelta * 2000.f;
	
	if (m_fSizeX >= 6000.f)
		Destory();
	
	if (m_fSizeY >= 6000.f)
		Destory();
	
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.1f);
}

void CUI_VS_DynamicLight::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	//RENDER_OBJECT tDesc{};
	//tDesc.tGlowDesc.iPassIndex = 2;
	//tDesc.tGlowDesc.fGlowFactor = 5.2f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_DynamicLight::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(29)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_DynamicLight::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_DynamicLight"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_VS_DynamicLight* CUI_VS_DynamicLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_DynamicLight* pInstatnce = new CUI_VS_DynamicLight(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_DynamicLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_DynamicLight::Clone(void* pArg)
{
	CUI_VS_DynamicLight* pInstatnce = new CUI_VS_DynamicLight(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		//MSG_BOX(TEXT("Failed to Cloend : CUI_VS_DynamicLight"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_DynamicLight::Free()
{
	__super::Free();
}
