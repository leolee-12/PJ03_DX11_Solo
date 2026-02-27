#include "stdafx.h"

#include "UI_KOFontEffect.h"
#include "RenderInstance.h"

CUI_KOFontEffect::CUI_KOFontEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_KOFontEffect::CUI_KOFontEffect(const CUI_KOFontEffect& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_KOFontEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_KOFontEffect::Initialize(void* pArg)
{
	m_fSizeX = 800.f;
	m_fSizeY = 800.f;
	m_fPosX = 640;
	m_fPosY = 360;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);
	
	
	Set_AnimPosition(400, 0.5f);
	Set_AnimPosition(300, 0.55f);
	Set_AnimPosition(350, 0.65f);
	Set_AnimPosition(700, 0.7f);
	Set_AnimPosition(1000, 1.f);
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	m_fAlphaTimer = 1.f;

	return S_OK;
}

void CUI_KOFontEffect::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_fAlphaTimer -= fTimeDelta;// *0.5f;
	if (m_fAlphaTimer <= 0.f)
	{
		m_fAlphaTimer = 0.f;
	}
	//m_pUI_Manager->m_fTotalDuration += fTimeDelta;
	
}

void CUI_KOFontEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(m_QueueAnim.empty())
		Destory();
	



	Action_Anim(1.f, fTimeDelta);

}

void CUI_KOFontEffect::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_KOFontEffect::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_KOFontEffect::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameKOFontEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_KOFontEffect::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	
	_float fAlpha = 0.5f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaTimer, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &fTmp, sizeof(_float))))
	//	return E_FAIL;

	return S_OK;
}


CUI_KOFontEffect* CUI_KOFontEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_KOFontEffect* pInstatnce = new CUI_KOFontEffect(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_KOFontEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_KOFontEffect::Clone(void* pArg)
{
	CUI_KOFontEffect* pInstatnce = new CUI_KOFontEffect(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_KOFontEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_KOFontEffect::Free()
{

	__super::Free();
}
