
#include "stdafx.h"

#include "UI_SubHpGauge.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_SubHpGauge::CUI_SubHpGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_SubHpGauge::CUI_SubHpGauge(const CUI_SubHpGauge& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_SubHpGauge::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SubHpGauge::Initialize(void* pArg)
{
	m_fPosX = 237.f;
	m_fPosY = 103.f;
	m_fSizeX = 194.4;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, 18, m_fPosX, m_fPosY, 0.75f);

	return S_OK;
}

void CUI_SubHpGauge::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

}

void CUI_SubHpGauge::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
	(m_pSubPawn != nullptr) ? m_fHpRadio = m_pSubPawn->Get_PawnDesc().iHp / 10000.f : Destory();

	(m_fHpRadio >= 1.f) ? m_iShaderID = 11 : m_iShaderID = 5;

	Animation({ 250.f , 67.f ,0.8, 1.f }, { m_fPosX, m_fPosY, 0.8f, 1.f }, 500.f, 0.8f, fTimeDelta);
}

void CUI_SubHpGauge::Late_Update(_float fTimeDelta)
{
	if (m_fHpRadio > 0.f)
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_SubHpGauge::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(m_iShaderID)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SubHpGauge::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpGauge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_SubHpGauge::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Radio", &m_fHpRadio, sizeof(_float))))
		return E_FAIL;

	_vector vColor = { 0.043f , 0.952f , 0.945 , 0.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL; 

	return S_OK;
}

CUI_SubHpGauge* CUI_SubHpGauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SubHpGauge* pInstatnce = new CUI_SubHpGauge(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SubHpGauge"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_SubHpGauge::Clone(void* pArg)
{
	CUI_SubHpGauge* pInstatnce = new CUI_SubHpGauge(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_SubHpGauge"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_SubHpGauge::Free()
{
	__super::Free();
}
