#include "stdafx.h"

#include "UI_Loading_DragonBall.h"
#include "RenderInstance.h"

#include "UI_Loading_FlyEff.h"

CUI_Loading_DragonBall::CUI_Loading_DragonBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Loading_DragonBall::CUI_Loading_DragonBall(const CUI_Loading_DragonBall& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Loading_DragonBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_DragonBall::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iTextureIndex = pDesc->iNumUI;

	m_fSizeX = 70.f, m_fSizeY = 70.f;
	m_fPosX = 300.f , m_fPosY = 600.f;

	InitPosition();

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX , m_fPosY, 0.f);

	return S_OK;
}

void CUI_Loading_DragonBall::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Loading_DragonBall::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Loading_DragonBall::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	_bool bIsEmpty = m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")).empty();

	if (bIsEmpty == FALSE)
	{
		m_bFinishEnd =  dynamic_cast<CUI_Loading_FlyEff*>(m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")).back())->Get_AnimEnd();
	}

	if (m_bFinishEnd && m_pUI_Manager->m_iNumThreadFinish >= 7 && m_bFinishAnim == FALSE)
	{
			m_fBlurValue = 0.f;
			m_bFinishAnim = TRUE;

		if(m_iTextureIndex == 0)
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOADING_BALL_FINISH_SFX, false, 0.2f);
	}

	if (m_iDefTextureIndex != 7 && m_pUI_Manager->m_iNumThreadFinish < 7)
	{
		if (m_fBlurValue >= 2.5f)
			m_bBlurSwitch = FALSE;
		else if (m_fBlurValue <= 0.f)
			m_bBlurSwitch = TRUE;

		m_bBlurSwitch ? m_fBlurValue += fTimeDelta * 2.5f : m_fBlurValue -= fTimeDelta * 2.5f;
	}

	if (m_bFinishAnim)
	{
		if(m_bMaskValueSwitch == FALSE)
			m_fMaskValue += fTimeDelta * 2.f;

		if (m_fMaskValue >= 1.f)
			m_bMaskValueSwitch = TRUE;

		if(m_bMaskValueSwitch)
			m_fMaskValue -= fTimeDelta * 2.f;

		if (m_fMaskValue < 0.f)
		{
			m_fMaskValue = 0.f;

			m_pUI_Manager->m_bGamePlayLoadingFinish = TRUE;
		}
	}

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = m_fBlurValue;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_MULTY_GLOW, this,&tDesc);
}

HRESULT CUI_Loading_DragonBall::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(33)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_DragonBall::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingDragonBall"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Loading_DragonBall::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	for (auto& pAuto : m_pGameInstance->Get_Layer(LEVEL_LOADING, TEXT("Layer_UI_LoadingFlyEff")))
	{
		CUI_Loading_FlyEff* pFlyEff = dynamic_cast<CUI_Loading_FlyEff*>(pAuto);

		_bool bAnimEnd = pFlyEff->Get_AnimEnd();
		if (pFlyEff->Get_ThreadID() == m_iTextureIndex && bAnimEnd)
		{
			m_iDefTextureIndex = m_iTextureIndex;
		}
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDefTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fMaskValue, sizeof(_float))))
		return E_FAIL;


	

	return S_OK;
}

void CUI_Loading_DragonBall::InitPosition()
{
	if (m_iTextureIndex % 2 == 0)
		m_fPosY += 40;

	m_fPosX += m_iTextureIndex * 100.f;
}

CUI_Loading_DragonBall* CUI_Loading_DragonBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_DragonBall* pInstatnce = new CUI_Loading_DragonBall(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Loading_DragonBall"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Loading_DragonBall::Clone(void* pArg)
{
	CUI_Loading_DragonBall* pInstatnce = new CUI_Loading_DragonBall(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Loading_DragonBall"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Loading_DragonBall::Free()
{
	__super::Free();
}
