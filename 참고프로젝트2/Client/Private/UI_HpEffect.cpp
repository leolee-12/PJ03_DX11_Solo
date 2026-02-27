#include "stdafx.h"

#include "UI_HpEffect.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_HpEffect::CUI_HpEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_HpEffect::CUI_HpEffect(const CUI_HpEffect& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_HpEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HpEffect::Initialize(void* pArg)
{
	m_fPosX = 330.f;
	m_fSizeX = 464.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX * 1.01f , 116.f * 1.01f, m_fPosX, 67.f, 0.f);
	//__super::Set_UI_Setting(m_fSizeX * 1.01f, 116.f * 1.01f, m_fPosX, 150.f, 0.f);

	return S_OK;
}

void CUI_HpEffect::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
	
	if (m_pMainPawn != nullptr && m_pMainPawn->Get_PawnDesc().iHp / 10000.f <= 0.25f)
	{
		if (m_fAlphaTimer <= 0.f)
		{
			m_bSign = TRUE;
		}
		else if (m_fAlphaTimer >= 0.5f)
		{
			m_bSign = FALSE;
		}

		m_bSign ? m_fAlphaTimer += fTimeDelta : m_fAlphaTimer -= fTimeDelta;
	}
	else
		m_fAlphaTimer = 0.f;
	
	
}

void CUI_HpEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CUI_HpEffect::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 4.7f;


	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI_GLOW, this, &tDesc);
}

HRESULT CUI_HpEffect::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_vector vColor = { 1.f , 0.f , 0.f , 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaTimer, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(13)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HpEffect::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_HpEffect* CUI_HpEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HpEffect* pInstatnce = new CUI_HpEffect(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_HpEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_HpEffect::Clone(void* pArg)
{
	CUI_HpEffect* pInstatnce = new CUI_HpEffect(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_HpEffect"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_HpEffect::Free()
{
	__super::Free();
}
