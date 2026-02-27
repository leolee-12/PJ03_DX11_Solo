#include "stdafx.h"

#include "UI_ComboNumber.h"
#include "RenderInstance.h"
#include "UI_Define.h"
#include "Character.h"

CUI_ComboNumber::CUI_ComboNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Combo{ pDevice ,pContext }
{
}

CUI_ComboNumber::CUI_ComboNumber(const CUI_ComboNumber& Prototype)
	:CUI_Combo{ Prototype }
{
}

HRESULT CUI_ComboNumber::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ComboNumber::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pComboDesc = static_cast<UI_DESC*>(pArg);
	m_iNumUI = pComboDesc->iNumUI;
	m_eLRPos = pComboDesc->eLRPos;

	InitPosition();

	return S_OK;
}

void CUI_ComboNumber::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
	


}

void CUI_ComboNumber::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	//숫자가 나타날 때 스케일 애니메이션
	ScaleAnimation();

	//히트가 끝날 때 이펙트
	EndAlphaEffect(fTimeDelta);
}

void CUI_ComboNumber::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	
	if(RenderEnable(m_iLimitComboCount))
		m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_ComboNumber::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ComboNumber::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ComboNumber"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_ComboNumber::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", Return_TextureIndex()  + SetColor() * 10)))
		return E_FAIL;

	_vector vColor = { 0.f, 0.f, 0.f, 1.f };

	if (m_bCharaStun == FALSE && m_iComboCount >= 2)
	{
		 vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else
		vColor = { 0.f, 0.f, 0.f, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &vColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskTimer", &m_fAlphaTimer, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_ComboNumber::InitPosition()
{
	switch (m_iNumUI)
	{
	case FIRST:
		m_fSizeX = 81.f, m_fSizeY = 108.f, m_fPosX = 96.f, m_fPosY = 238.f, m_fDepth = 0.5f;
		m_iLimitComboCount = 100;
		break;

	case SECOND:
		m_fSizeX = 81.f, m_fSizeY = 108.f, m_fPosX = 150.f, m_fPosY = 238.f, m_fDepth = 0.6f;
		m_iLimitComboCount = 10;
		break;

	case THIRD:
		m_fSizeX = 81.f, m_fSizeY = 108.f, m_fPosX = 204.f, m_fPosY = 238.f, m_fDepth = 0.7f;
		m_iLimitComboCount = 2;
		break;
	}

	if (m_eLRPos == LEFT)
	{
		m_fPosX += 1280 * 0.5f + 300.f;
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, m_fDepth);
}

_uint CUI_ComboNumber::Return_TextureIndex()
{
	switch (m_iNumUI)
	{
	case FIRST:
		return (m_iComboCount / 100);
		break;

	case SECOND:
		return ((m_iComboCount / 10) % 10);
		break;

	case THIRD:
		return  (m_iComboCount % 10);
		break;
	}

	return 0;
}

void CUI_ComboNumber::ScaleAnimation()
{
	_bool bHit = FALSE;

	if (m_iPrevCombo < m_iComboCount)
		m_bScaleAnim = TRUE;

	if (m_bScaleAnim)
	{
		__super::Set_UI_Setting(m_fSizeX * 1.25f, m_fSizeY * 1.25f, m_fPosX, m_fPosY, m_fDepth);
		m_bScaleAnim = FALSE;
	}
	else 
	{
		__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, m_fDepth);
	}

	m_iPrevCombo = m_iComboCount;


}

void CUI_ComboNumber::EndAlphaEffect(_float fTimeDelta)
{
	if (m_bCharaStun == FALSE && m_iComboCount >= 2)
	{
		CCharacter::Character_INFO_DESC Desc =  m_pMainPawn->Get_PawnDesc();

		m_fAlphaTimer += fTimeDelta *  5.f;

		if (m_fAlphaTimer >= 1.f)
			m_fAlphaTimer = 1.f;
	}
	else
		m_fAlphaTimer = 0.f;
}

_bool CUI_ComboNumber::RenderEnable(_uint iLimit)
{
	if (m_iComboCount >= iLimit)
		return TRUE;

	return FALSE;
}

CUI_ComboNumber* CUI_ComboNumber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ComboNumber* pInstatnce = new CUI_ComboNumber(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_ComboNumber"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_ComboNumber::Clone(void* pArg)
{
	CUI_ComboNumber* pInstatnce = new CUI_ComboNumber(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_ComboNumber"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_ComboNumber::Free()
{
	__super::Free();
}
