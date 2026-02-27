#include "stdafx.h"

#include "UI_CharaSelectCircle.h"
#include "RenderInstance.h"

CUI_CharaSelectCircle::CUI_CharaSelectCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_CharaSelectCircle::CUI_CharaSelectCircle(const CUI_CharaSelectCircle& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_CharaSelectCircle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectCircle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	UI_DESC* Desc = static_cast<UI_DESC*>(pArg);
	_uint iNum =  Desc->iNumUI;

	m_fSizeX = 600 * (iNum + 1 ), m_fSizeY = 300 * (iNum + 1);
	m_fPosX = 630, m_fPosY = 160;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_CharaSelectCircle::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_CharaSelectCircle::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	Play_Animation(fTimeDelta);
}

void CUI_CharaSelectCircle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_CharaSelectCircle::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(26)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_CharaSelectCircle::Play_Animation(_float fTimeDelta)
{
	m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, fTimeDelta);

	if (m_fScaledValue >= 150.f)
		m_bValueSwitch = FALSE;
	else if (m_fScaledValue <= 0.f)
		m_bValueSwitch = TRUE;

	m_bValueSwitch ? m_fScaledValue += fTimeDelta * 40.f : m_fScaledValue -= fTimeDelta * 40.f;

	__super::Set_UI_Setting(m_fSizeX + m_fScaledValue, m_fSizeY + m_fScaledValue, m_fPosX, m_fPosY, 0.f);
}

HRESULT CUI_CharaSelectCircle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectCircle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_CharaSelectCircle* CUI_CharaSelectCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharaSelectCircle* pInstatnce = new CUI_CharaSelectCircle(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_CharaSelectCircle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_CharaSelectCircle::Clone(void* pArg)
{
	CUI_CharaSelectCircle* pInstatnce = new CUI_CharaSelectCircle(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_CharaSelectCircle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_CharaSelectCircle::Free()
{
	__super::Free();
}
