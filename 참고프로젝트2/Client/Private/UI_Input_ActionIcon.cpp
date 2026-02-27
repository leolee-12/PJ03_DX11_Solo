#include "stdafx.h"

#include "UI_Input_ActionIcon.h"
#include "RenderInstance.h"

CUI_Input_ActionIcon::CUI_Input_ActionIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_Input_ActionIcon::CUI_Input_ActionIcon(const CUI_Input_ActionIcon& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_Input_ActionIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_ActionIcon::Initialize(void* pArg)
{
	m_fPosX = 60.f, m_fPosY = 190.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 30.f, m_fSizeY = 30.f;

	UI_INPUT_DESC* pDirDesc = static_cast<UI_INPUT_DESC*>(pArg);
	m_iTextureIndex = pDirDesc->eActionInput;

	InputGrab(m_iTextureIndex);

	if (m_eLRPos == LEFT)
	{
		if (m_pUI_Manager->m_eDirInput == MOVEKEY_NEUTRAL)
		{
			m_fPosX = 20;
			if (m_pUI_Manager->m_eBtnInput != ATTACK_GRAB)
				m_pUI_Manager->m_iNumCommandList++;
		}

		if (pDirDesc->isGrab)
			InputGrabPosition(m_fPosX);

		m_iNumCommandList = m_pUI_Manager->m_iNumCommandList;
	}

	else if (m_eLRPos == RIGHT)
	{

		if (m_pUI_Manager->m_eDirInput2 == MOVEKEY_NEUTRAL)
		{
			m_fPosX = m_vPrevWinSize.x - 20.f;
			if (m_pUI_Manager->m_eBtnInput2 != ATTACK_GRAB)
				m_pUI_Manager->m_iNumCommandList2++;
		}

		if (pDirDesc->isGrab)
			InputGrabPosition(m_fPosX);

		m_iNumCommandList = m_pUI_Manager->m_iNumCommandList2;
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.8f);

	return S_OK;
}

void CUI_Input_ActionIcon::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

}

void CUI_Input_ActionIcon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_float fOffSetPosY = 0.f;

	if(m_eLRPos == LEFT)
		fOffSetPosY = m_fPosY + (40 * (m_pUI_Manager->m_iNumCommandList - m_iNumCommandList));

	else if (m_eLRPos == RIGHT)
		fOffSetPosY = m_fPosY + (40 * (m_pUI_Manager->m_iNumCommandList2 - m_iNumCommandList));

	if (fOffSetPosY >= 575)
		Destory();

	__super::Set_UI_Setting(30.f, 30.f, m_fPosX, fOffSetPosY, 0.8f);
}

void CUI_Input_ActionIcon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	//if(m_iTextureIndex)
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Input_ActionIcon::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex - 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_ActionIcon::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ActionInput"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_Input_ActionIcon::InputGrab(_uint iTexIndex)
{
	if (m_iTextureIndex == ATTACK_GRAB)
	{
		m_iTextureIndex = ATTACK_LIGHT;

		UI_INPUT_DESC InputDesc = {};
		InputDesc.isGrab = TRUE;
		InputDesc.eActionInput = ATTACK_MEDIUM;
		InputDesc.eLRPos = m_eLRPos;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ActionInputIcon"), TEXT("Layer_DirInput"), &InputDesc);
	}


}

void CUI_Input_ActionIcon::InputGrabPosition(_float& fPos)
{
	if(m_eLRPos == LEFT)
		fPos += 40;
	else if(m_eLRPos == RIGHT)
		fPos -= 40;

	if (m_pUI_Manager->m_eDirInput == MOVEKEY_NEUTRAL && m_eLRPos == LEFT)
			m_pUI_Manager->m_iNumCommandList++;

	else if (m_pUI_Manager->m_eDirInput2 == MOVEKEY_NEUTRAL && m_eLRPos == RIGHT)
			m_pUI_Manager->m_iNumCommandList2++;
}

CUI_Input_ActionIcon* CUI_Input_ActionIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Input_ActionIcon* pInstatnce = new CUI_Input_ActionIcon(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Input_ActionIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Input_ActionIcon::Clone(void* pArg)
{
	CUI_Input_ActionIcon* pInstatnce = new CUI_Input_ActionIcon(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Input_ActionIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Input_ActionIcon::Free()
{
	__super::Free();
}
