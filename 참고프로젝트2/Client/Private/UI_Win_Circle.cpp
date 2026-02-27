#include "stdafx.h"

#include "UI_Win_Circle.h"
#include "RenderInstance.h"

CUI_Win_Circle::CUI_Win_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Win_Circle::CUI_Win_Circle(const CUI_Win_Circle& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Win_Circle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Win_Circle::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 200.f;
	m_fSizeY = 200.f;
	m_fPosX = 50.f, m_fPosY = 600.f;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iTextureIndex = pDesc->iNumUI;

	m_pUI_Manager->m_bChange[0] = TRUE;



	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Win_Circle::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Win_Circle::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fDestroy += fTimeDelta;
	
	if (m_fDestroy >= 5.f)
	{
		//Destory();
	}
	if (m_iTextureIndex != 0)
	{
		_float iOffsetRotation = 0;
		m_iTextureIndex % 2 == 0 ? iOffsetRotation = 1 : iOffsetRotation = -1;
		m_pTransformCom->Turn({ 0,0,iOffsetRotation }, fTimeDelta);
	}

	Animation({ m_fPosX - 500.f , m_fPosY + 500.f  ,0.f, 1.f }, { m_fPosX, m_fPosY, 0.f, 1.f }, 2000.f, 0.f, fTimeDelta, 30.f);

}

void CUI_Win_Circle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Win_Circle::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

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

HRESULT CUI_Win_Circle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingMark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Win_Circle* CUI_Win_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Win_Circle* pInstatnce = new CUI_Win_Circle(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Win_Circle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Win_Circle::Clone(void* pArg)
{
	CUI_Win_Circle* pInstatnce = new CUI_Win_Circle(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Win_Circle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Win_Circle::Free()
{
	__super::Free();
}
