#include "stdafx.h"

#include "UI_KOParticle.h"
#include "RenderInstance.h"

CUI_KOParticle::CUI_KOParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_KOParticle::CUI_KOParticle(const CUI_KOParticle& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_KOParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_KOParticle::Initialize(void* pArg)
{
	m_fSizeX = 800.f;
	m_fSizeY = 400.f;
	m_fPosX = 640;
	m_fPosY = 360;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_KOParticle::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fTexCoord += fTimeDelta * 0.5f;

	
}

void CUI_KOParticle::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_fTexCoord >= 1.f)
	{
		m_fTexCoord = 0.f;
		Destory();
	}

	Action_Anim(1.f, fTimeDelta);


}

void CUI_KOParticle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 3.7f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI_GLOW, this, &tDesc);
}

HRESULT CUI_KOParticle::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;
	//16
	if (FAILED(m_pShaderCom->Begin(16)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_KOParticle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_KOParticle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_KOParticle::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fTexCoord, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CUI_KOParticle* CUI_KOParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_KOParticle* pInstatnce = new CUI_KOParticle(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_KOParticle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_KOParticle::Clone(void* pArg)
{
	CUI_KOParticle* pInstatnce = new CUI_KOParticle(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_KOParticle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_KOParticle::Free()
{

	__super::Free();
}
