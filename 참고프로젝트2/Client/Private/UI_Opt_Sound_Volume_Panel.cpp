#include "stdafx.h"

#include "UI_Opt_Sound_Volume_Panel.h"
#include "RenderInstance.h"

CUI_Opt_Sound_Volume_Panel::CUI_Opt_Sound_Volume_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Opt_Sound{ pDevice ,pContext }
{
}

CUI_Opt_Sound_Volume_Panel::CUI_Opt_Sound_Volume_Panel(const CUI_Opt_Sound_Volume_Panel& Prototype)
	:CUI_Opt_Sound{ Prototype }
{
}

HRESULT CUI_Opt_Sound_Volume_Panel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound_Volume_Panel::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = 790.f, m_fPosY = 320.f;
	m_fSizeX = 266.f, m_fSizeY = 43.f;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iNumUI = pDesc->iNumUI;

	PostionUpdate();

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Opt_Sound_Volume_Panel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Opt_Sound_Volume_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_bKeyInput)
		PostionUpdate();
}

void CUI_Opt_Sound_Volume_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Opt_Sound_Volume_Panel::Render(_float fTimeDelta)
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

	CreateNameFont();

	return S_OK;
}

HRESULT CUI_Opt_Sound_Volume_Panel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptVolumePanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_Opt_Sound_Volume_Panel::PostionUpdate()
{
	switch (m_iNumUI)
	{
	case BGM:
		m_fPosY = 320.f;
		break;

	case VOICE:
		m_fPosY = 360.f;
		break;

	case SFX:
		m_fPosY = 400.f;
		break;
	
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);
}

void CUI_Opt_Sound_Volume_Panel::CreateNameFont()
{
	_wstring strName = TEXT("Empty");
	_vector vColor = {};


		switch (m_iNumUI)
		{
		case BGM:
			strName = TEXT("Music Volume");
			break;

		case SFX:
			strName = TEXT("SFX Volume");
			break;

		case VOICE:
			strName = TEXT("Voice Volume");
			break;

		}

		(m_iNumUI == m_eMenuValue) ? vColor = { 1.f, 1.f, 1.f ,1.f } : vColor = { 0.996f, 0.729f, 0.f ,1.f };
		
	m_pGameInstance->Draw_Font(TEXT("Font_Nexon"),
		strName.c_str(),
		_float2((m_fPosX - 380) * 1.5f, (m_fPosY - 10) * 1.5f),
		vColor,
		0.f,
		{ 0.f, 0.f },
		0.8f
	);
}


CUI_Opt_Sound_Volume_Panel* CUI_Opt_Sound_Volume_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Opt_Sound_Volume_Panel* pInstatnce = new CUI_Opt_Sound_Volume_Panel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Opt_Sound_Volume_Panel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Opt_Sound_Volume_Panel::Clone(void* pArg)
{
	CUI_Opt_Sound_Volume_Panel* pInstatnce = new CUI_Opt_Sound_Volume_Panel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Opt_Sound_Volume_Panel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Opt_Sound_Volume_Panel::Free()
{
	__super::Free();
}
