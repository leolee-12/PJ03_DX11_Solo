#include "stdafx.h"

#include "UI_Opt_Sound_Font.h"
#include "RenderInstance.h"

CUI_Opt_Sound_Font::CUI_Opt_Sound_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Opt_Sound{ pDevice ,pContext }
{
}

CUI_Opt_Sound_Font::CUI_Opt_Sound_Font(const CUI_Opt_Sound_Font& Prototype)
	:CUI_Opt_Sound{ Prototype }
{
}

HRESULT CUI_Opt_Sound_Font::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound_Font::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = 502.f, m_fPosY = 277.f;
	m_fSizeX = 320.f, m_fSizeY = 40.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_Opt_Sound_Font::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Opt_Sound_Font::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Opt_Sound_Font::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Opt_Sound_Font::Render(_float fTimeDelta)
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

	_vector vColor = { 0.996f, 0.729f, 0.f ,1.f };
	(EXIT == m_eMenuValue) ? vColor = { 1.f, 1.f, 1.f ,1.f } : vColor = { 0.996f, 0.729f, 0.f ,1.f };


	m_pGameInstance->Draw_Font(TEXT("Font_Nexon"),
		TEXT("Exit"),
		_float2((790.f - 380.f) * 1.5f, ((m_fPosY + 163.f) - 10.f) * 1.5f),
		vColor,
		0.f,
		{ 0.f, 0.f },
		0.8f
	);

	return S_OK;
}

HRESULT CUI_Opt_Sound_Font::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptSoundFont"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}



CUI_Opt_Sound_Font* CUI_Opt_Sound_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Opt_Sound_Font* pInstatnce = new CUI_Opt_Sound_Font(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Opt_Sound_Font"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Opt_Sound_Font::Clone(void* pArg)
{
	CUI_Opt_Sound_Font* pInstatnce = new CUI_Opt_Sound_Font(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Opt_Sound_Font"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Opt_Sound_Font::Free()
{
	__super::Free();
}
