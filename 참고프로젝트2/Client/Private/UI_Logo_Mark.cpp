#include "stdafx.h"

#include "UI_Logo_Mark.h"
#include "RenderInstance.h"

CUI_Logo_Mark::CUI_Logo_Mark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Logo_Mark::CUI_Logo_Mark(const CUI_Logo_Mark& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Logo_Mark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Logo_Mark::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 1508;
	m_fSizeY = 754;
	m_fPosX = 610;
	m_fPosY = 282;
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.0f);
	return S_OK;
}

void CUI_Logo_Mark::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Logo_Mark::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CUI_Logo_Mark::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Logo_Mark::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	PressingCreateFont(fTimeDelta);

	return S_OK;
}

HRESULT CUI_Logo_Mark::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Mark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CUI_Logo_Mark::PressingCreateFont(_float fTimeDelta)
{
	m_IsAlphaSwich ? (m_fAlphaValue += fTimeDelta * 0.75f) : (m_fAlphaValue -= fTimeDelta * 0.75f);

	if (m_fAlphaValue <= 0.25f)
		m_IsAlphaSwich = TRUE;
	else if (m_fAlphaValue >= 1.f)
		m_IsAlphaSwich = FALSE;

	m_pGameInstance->Draw_Font(TEXT("Font_Nexon"),
		TEXT("Space Bar Key Pressing"),
		_float2(450 * 1.5f, (600) * 1.5f),
		{ 1.f,1.f,1.f,m_fAlphaValue },
		0.f,
		{ 0.f, 0.f },
		1.f
	);
}

CUI_Logo_Mark* CUI_Logo_Mark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Logo_Mark* pInstatnce = new CUI_Logo_Mark(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Logo_Mark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Logo_Mark::Clone(void* pArg)
{
	CUI_Logo_Mark* pInstatnce = new CUI_Logo_Mark(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Logo_Mark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Logo_Mark::Free()
{
	__super::Free();
}
