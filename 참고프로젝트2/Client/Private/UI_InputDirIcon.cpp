#include "stdafx.h"

#include "UI_InputDirIcon.h"
#include "RenderInstance.h"

CUI_InputDirIcon::CUI_InputDirIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_InputDirIcon::CUI_InputDirIcon(const CUI_InputDirIcon& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_InputDirIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InputDirIcon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_INPUT_DESC* pDirDesc = static_cast<UI_INPUT_DESC*>(pArg);
	m_iTextureIndex = pDirDesc->eDirInput;

	m_fSizeX = 35.f;
	m_fSizeY = 35.f;
	m_fPosX = 20.f;
	m_fPosY = 190;


	if (m_eLRPos == LEFT)
	{
		m_pUI_Manager->m_iNumCommandList++;
		m_iNumCommandList  = m_pUI_Manager->m_iNumCommandList;
	}
	else if (m_eLRPos == RIGHT)
	{
		m_pUI_Manager->m_iNumCommandList2++;
		m_iNumCommandList = m_pUI_Manager->m_iNumCommandList2;
		m_fPosX = m_vPrevWinSize.x - 20.f;

		switch (m_iTextureIndex)
		{
		case 3:
			m_iTextureIndex = 4;
			break;
		case 4:
			m_iTextureIndex = 3;
			break;
		case 5:
			m_iTextureIndex = 6;
			break;
		case 6:
			m_iTextureIndex = 5;
			break;
		case 7:
			m_iTextureIndex = 8;
			break;
		case 8:
			m_iTextureIndex = 7;
			break;
		default:
			break;
		}
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.8f);

	return S_OK;
}

void CUI_InputDirIcon::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

}

void CUI_InputDirIcon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_float fOffSetPosY = 0.f;

	if (m_eLRPos == LEFT)
		fOffSetPosY = m_fPosY + (40 * (m_pUI_Manager->m_iNumCommandList - m_iNumCommandList));
	else if(m_eLRPos == RIGHT)
		fOffSetPosY = m_fPosY + (40 * (m_pUI_Manager->m_iNumCommandList2 - m_iNumCommandList));

	if (fOffSetPosY >= 575)
		Destory();

	__super::Set_UI_Setting(35.f, 35.f, m_fPosX, fOffSetPosY , 0.8f);
}

void CUI_InputDirIcon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_InputDirIcon::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InputDirIcon::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DirInputIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
		

	return S_OK;
}

CUI_InputDirIcon* CUI_InputDirIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_InputDirIcon* pInstatnce = new CUI_InputDirIcon(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_InputDirIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_InputDirIcon::Clone(void* pArg)
{
	CUI_InputDirIcon* pInstatnce = new CUI_InputDirIcon(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_InputDirIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_InputDirIcon::Free()
{

	__super::Free();
}
