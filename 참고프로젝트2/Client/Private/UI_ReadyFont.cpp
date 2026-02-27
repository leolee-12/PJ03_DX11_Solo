#include "stdafx.h"

#include "UI_ReadyFont.h"
#include "RenderInstance.h"

CUI_ReadyFont::CUI_ReadyFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_ReadyFont::CUI_ReadyFont(const CUI_ReadyFont& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_ReadyFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ReadyFont::Initialize(void* pArg)
{

	m_fSizeX = 400.f;
	m_fSizeY = 200.f;
	m_fPosX = 640;
	m_fPosY = 360;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);


	m_fOffSetPosY = 50.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY - m_fOffSetPosY, 0.f);

	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_READY, false, 0.5f);
	return S_OK;
}

void CUI_ReadyFont::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);


	
}

void CUI_ReadyFont::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Action_StartAnim(fTimeDelta);
	//Action_ScaleAnim(fTimeDelta);



}

void CUI_ReadyFont::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_ReadyFont::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ReadyFont::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameReadyFont"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_MaskTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameReadyFont"),
		TEXT("Com_MaskTexture "), reinterpret_cast<CComponent**>(&m_pMaskTexture))))
		return E_FAIL;	

	return S_OK;
}

HRESULT CUI_ReadyFont::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;


	//1번이 BG(노란색) 2번이 Main(검정색)
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fMaskTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DestroyTimer", &m_fAlphaTimer, sizeof(_float))))
		return E_FAIL;

	//m_fAlphaTimer 

	return S_OK;
}

void CUI_ReadyFont::Action_StartAnim(_float fTimeDelta)
{
	if (m_bStart == FALSE && m_fOffSetPosY >= 0.f)
	{
		m_fStartAnimTimer += fTimeDelta *25;

		m_fOffSetPosY -= m_fStartAnimTimer;

		__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY - m_fOffSetPosY, 0.f);
	}
	else
	{
		m_bStart = TRUE;
		m_fStartAnimTimer = 0.f;
	}

	if (m_bStart == TRUE && m_bEnd == FALSE)
	{
		m_fMaskTimer += fTimeDelta;

		if (m_fMaskTimer >= 1.f)
		{
			m_bEnd = TRUE;
			m_fMaskTimer = 1.f;

		}
	}
	if (m_bEnd)
	{
		m_fMaskTimer -= fTimeDelta ;
		m_fAlphaTimer += fTimeDelta * 10;
	}

	if (m_fAlphaTimer >= 2.f)
	{
		Destory();
		m_fAlphaTimer = 0.f;
		m_fAlphaTimer = 0.f;


		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_FightFont"), TEXT("Layer_UI_GameStartFont"));
	}

}


CUI_ReadyFont* CUI_ReadyFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ReadyFont* pInstatnce = new CUI_ReadyFont(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_ReadyFont"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_ReadyFont::Clone(void* pArg)
{
	CUI_ReadyFont* pInstatnce = new CUI_ReadyFont(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_ReadyFont"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_ReadyFont::Free()
{
	Safe_Release(m_pMaskTexture);

	__super::Free();
}
