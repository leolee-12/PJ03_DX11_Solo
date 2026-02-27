#include "stdafx.h"

#include "UI_VS_MarkEff.h"
#include "RenderInstance.h"

CUI_VS_MarkEff::CUI_VS_MarkEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_VS_MarkEff::CUI_VS_MarkEff(const CUI_VS_MarkEff& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_VS_MarkEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_MarkEff::Initialize(void* pArg)
{
	m_fSizeX = 400.f, m_fSizeY = 800.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fAlphaValue = 1.f;

	Set_AnimPosition(800.f, 0.1f);
	Set_AnimPosition(400.f, 0.5f);
	Set_AnimPosition(300.f, 0.75f);
	Set_AnimPosition(300.f, 0.85f);
	Set_AnimPosition(700.f, 0.9f);
	Set_AnimPosition(1000.f, 1.f);

	m_fPosX = 630.f, m_fPosY = 305.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_MarkEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_MarkEff::Update(_float fTimeDelta)
{
	if (fTimeDelta >= 0.1f)
		fTimeDelta = 0.f;

	__super::Update(fTimeDelta);

	if (m_pUI_Manager->m_fTotalDuration >= 0.5f)
	{
		m_fAlphaValue -= fTimeDelta;	
	}

	if (m_QueueAnim.empty())
		Destory();

	Action_Anim(2.f, fTimeDelta);
}

void CUI_VS_MarkEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_bDrawSwich || m_pUI_Manager->m_fTotalDuration >= 0.5f)
	{
		m_bDrawSwich = TRUE;
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
	}
}

HRESULT CUI_VS_MarkEff::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	_vector vColor = { 1.f , 1.f , 1.f ,1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(13)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_MarkEff::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_Mark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_VS_MarkEff* CUI_VS_MarkEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_MarkEff* pInstatnce = new CUI_VS_MarkEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_MarkEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_MarkEff::Clone(void* pArg)
{
	CUI_VS_MarkEff* pInstatnce = new CUI_VS_MarkEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_MarkEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_MarkEff::Free()
{
	__super::Free();
}
