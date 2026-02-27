#include "stdafx.h"

#include "UI_Win_Team.h"
#include "RenderInstance.h"

CUI_Win_Team::CUI_Win_Team(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Win_Team::CUI_Win_Team(const CUI_Win_Team& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Win_Team::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Win_Team::Initialize(void* pArg)
{
	m_fSizeX = 320.f, m_fSizeY = 40.f, m_fPosX = 140.f, m_fPosY = 630.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pUI_Manager->m_bChange[0] = TRUE;
	m_pTransformCom->Rotation({ 0.f ,0.f, 1.f }, XMConvertToRadians(15.f));

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.1f);

	return S_OK;
}

void CUI_Win_Team::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

}

void CUI_Win_Team::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	DestroyTimer += fTimeDelta;

	Animation({ m_fPosX - 500.f , m_fPosY + 500.f  ,0.f, 1.f }, { m_fPosX, m_fPosY, 0.f, 1.f }, 2000.f, 0.f, fTimeDelta, 30.f);

}

void CUI_Win_Team::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Win_Team::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	_float fAlpha = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Win_Team::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Team"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Win_Team* CUI_Win_Team::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Win_Team* pInstatnce = new CUI_Win_Team(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Win_Team"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Win_Team::Clone(void* pArg)
{
	CUI_Win_Team* pInstatnce = new CUI_Win_Team(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Win_Team"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Win_Team::Free()
{
	__super::Free();
}
