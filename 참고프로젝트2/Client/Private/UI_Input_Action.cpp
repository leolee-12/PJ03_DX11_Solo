#include "stdafx.h"

#include "UI_Input_Action.h"
#include "RenderInstance.h"

CUI_Input_Action::CUI_Input_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_Input_Action::CUI_Input_Action(const CUI_Input_Action& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_Input_Action::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_Action::Initialize(void* pArg)
{

	m_fPosX = 60.f, m_fPosY = 190.f;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);
	m_iTextureIndex  = pUI_Desc->iNumUI;
	m_fSizeX = 30.f, m_fSizeY = 30.f;
	switch (m_iTextureIndex)
	{
	case KEY_LIGHT:
		m_fPosX = 250.f;
		m_fPosY = 550.f;
		break;
	case KEY_MEDIUM:
		m_fPosX = 290.f;
		m_fPosY = 510.f;
		break;
	case KEY_SPECIAL:
		m_fPosX = 330.f;
		m_fPosY = 550.f;
		break;
	case KEY_HEAVY:
		m_fPosX = 250.f;
		m_fPosY = 590.f;
		break;

	case KEY_A1:
		m_fPosX = 290.f;
		m_fPosY = 590.f;
		break;

	case KEY_A2:
		m_fPosX = 330.f;
		m_fPosY = 590.f;
		break;
	}

	if(pUI_Desc->eLRPos == RIGHT)
		m_fPosX += m_vPrevWinSize.x * 0.5f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_Input_Action::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	if(m_bOnBtn)
		m_fBtnOffTimer += fTimeDelta;
}

void CUI_Input_Action::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	ButtonInput eActionInput = ATTACK_NONE;

	if(m_eLRPos == LEFT)
		eActionInput = m_pUI_Manager->m_eBtnInput;
	else if(m_eLRPos == RIGHT)
		eActionInput = m_pUI_Manager->m_eBtnInput2;

	if (  (eActionInput == ATTACK_GRAB && m_iTextureIndex <= 1 || m_bOnBtn))
	{
		m_fColor = 1.f;
		m_bOnBtn = TRUE;
	}

	if (eActionInput != ATTACK_GRAB && (eActionInput - 1 == m_iTextureIndex || m_bOnBtn))
	{
		m_fColor = 1.f;
		m_bOnBtn = TRUE;
	}
	else
	{
		m_fColor = 0.62f;
	}

	if (m_fBtnOffTimer >= 0.25f)
	{
		m_fBtnOffTimer = 0.f;
		m_bOnBtn = FALSE;
	}

	m_bOnBtn ? __super::Set_UI_Setting(m_fSizeX * 1.25f, m_fSizeY * 1.25f, m_fPosX, m_fPosY, 0.9f) : __super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);;
}

void CUI_Input_Action::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Input_Action::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	_vector vColor = { m_fColor, m_fColor, m_fColor, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_Action::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ActionInput"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Input_Action* CUI_Input_Action::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Input_Action* pInstatnce = new CUI_Input_Action(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Input_Action"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Input_Action::Clone(void* pArg)
{
	CUI_Input_Action* pInstatnce = new CUI_Input_Action(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Input_Action"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Input_Action::Free()
{
	__super::Free();
}
