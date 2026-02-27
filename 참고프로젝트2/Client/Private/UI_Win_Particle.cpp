#include "stdafx.h"

#include "UI_Win_Particle.h"
#include "RenderInstance.h"

CUI_Win_Particle::CUI_Win_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_Win_Particle::CUI_Win_Particle(const CUI_Win_Particle& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_Win_Particle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Win_Particle::Initialize(void* pArg)
{
	m_fSizeX = 300.f;
	m_fSizeY = 150.f;
	m_fPosX = 141.f;
	m_fPosY = 550.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);

	Set_AnimPosition(200, 0.25f);
	Set_AnimPosition(250, 0.75f);
	Set_AnimPosition(300, 1.f);
		
	m_pTransformCom->Rotation({ 0.f ,0.f, 1.f }, XMConvertToRadians(45.f));
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Win_Particle::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
	


}

void CUI_Win_Particle::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	Action_Anim(0.5f, fTimeDelta);

	m_pUI_Manager->m_fTotalDuration += fTimeDelta;
	m_fTexCoord -= fTimeDelta;

	if (m_QueueAnim.empty())
	{
		//Destory();
		m_pUI_Manager->m_fTotalDuration = 0.f;
	}
}

void CUI_Win_Particle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 12.7f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI_GLOW, this, &tDesc);
}

HRESULT CUI_Win_Particle::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;
	//16
	if (FAILED(m_pShaderCom->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Win_Particle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_KOParticle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Win_Particle::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fTexCoord, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CUI_Win_Particle* CUI_Win_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Win_Particle* pInstatnce = new CUI_Win_Particle(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Win_Particle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Win_Particle::Clone(void* pArg)
{
	CUI_Win_Particle* pInstatnce = new CUI_Win_Particle(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Win_Particle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Win_Particle::Free()
{

	__super::Free();
}
