#include "stdafx.h"

#include "UI_Input_DirPanel.h"
#include "RenderInstance.h"

CUI_Input_DirPanel::CUI_Input_DirPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_Input_DirPanel::CUI_Input_DirPanel(const CUI_Input_DirPanel& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_Input_DirPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Input_DirPanel::Initialize(void* pArg)
{
	m_fSizeX = 150; 
	m_fSizeY = 150;
	m_fPosX = 150;
	m_fPosY = 550;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_Input_DirPanel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Input_DirPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_uint iNum = 0;
	_uint iNum2= 0;

	if(m_eLRPos == LEFT)
		iNum = CreateList(m_eLRPos);
	else if (m_eLRPos == RIGHT)
		iNum2 = CreateList2(m_eLRPos);

	if (iNum >= 1 && m_eLRPos == LEFT)
	{
		UI_DESC UI_Desc = {};
		UI_Desc.iNumUI = iNum;
		UI_Desc.eLRPos = m_eLRPos;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputIconPanel"), TEXT("Layer_DirInput"), &UI_Desc);
	}

	if (iNum2 >= 1 && m_eLRPos == RIGHT)
	{
		UI_DESC UI_Desc = {};
		UI_Desc.iNumUI = iNum2;
		UI_Desc.eLRPos = m_eLRPos;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputIconPanel"), TEXT("Layer_DirInput"), &UI_Desc);
	}
}

void CUI_Input_DirPanel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Input_DirPanel::Render(_float fTimeDelta)
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

	return S_OK;
}

HRESULT CUI_Input_DirPanel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DirInputPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Input_DirPanel* CUI_Input_DirPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Input_DirPanel* pInstatnce = new CUI_Input_DirPanel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Input_DirPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Input_DirPanel::Clone(void* pArg)
{
	CUI_Input_DirPanel* pInstatnce = new CUI_Input_DirPanel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Input_DirPanel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Input_DirPanel::Free()
{
	__super::Free();
}
