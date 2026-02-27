#include "stdafx.h"

#include "UI_Chara_SubIcon.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_Chara_SubIcon::CUI_Chara_SubIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Chara_SubIcon::CUI_Chara_SubIcon(const CUI_Chara_SubIcon& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Chara_SubIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chara_SubIcon::Initialize(void* pArg)
{
	m_fPosX = 82.f;
	m_fPosY = 72.f;
	m_fSizeY = 115.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 115.f;
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.5f);

	return S_OK;
}

void CUI_Chara_SubIcon::Camera_Update(_float fTimeDelta)
{
	
}

void CUI_Chara_SubIcon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	(m_pSubPawn != nullptr) ? m_iCharaID = m_pSubPawn->Get_PawnDesc().ePlayerID : Destory();
	Animation({ 60.f ,35.f ,0.5f, 1.f }, { m_fPosX, m_fPosY, 0.5f, 1.f }, 500.f, 0.5f, fTimeDelta);
}

void CUI_Chara_SubIcon::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Chara_SubIcon::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chara_SubIcon::Ready_Components()
{
	if(FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CharaIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Chara_SubIcon::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCharaID)))
		return E_FAIL;

	if (m_pSubPawn != nullptr)
	{
		_float fHpRadio = m_pSubPawn->Get_PawnDesc().iHp / 10000.f;
		_bool bZeroHpCheck = FALSE;

		(fHpRadio <= 0.f) ? bZeroHpCheck = TRUE : bZeroHpCheck = FALSE;
	
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bState", &bZeroHpCheck, sizeof(_bool))))
			return E_FAIL;
	}
	return S_OK;
}

CUI_Chara_SubIcon* CUI_Chara_SubIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chara_SubIcon* pInstatnce = new CUI_Chara_SubIcon(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Chara_SubIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Chara_SubIcon::Clone(void* pArg)
{
	CUI_Chara_SubIcon* pInstatnce = new CUI_Chara_SubIcon(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Chara_SubIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Chara_SubIcon::Free()
{
	__super::Free();
}
