#include "stdafx.h"

#include "UI_LoadingMark.h"
#include "RenderInstance.h"

CUI_LoadingMark::CUI_LoadingMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_LoadingMark::CUI_LoadingMark(const CUI_LoadingMark& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_LoadingMark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingMark::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 200.f;
	m_fSizeY = 200.f;
	m_fPosX = m_vPrevWinSize.x - 100;
	m_fPosY = m_vPrevWinSize.y - 100;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iTextureIndex = pDesc->iNumUI;


	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_LoadingMark::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_LoadingMark::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_iTextureIndex != 0)
	{
		_float iOffsetRotation = 0;
		m_iTextureIndex % 2 == 0 ? iOffsetRotation = 1 : iOffsetRotation = -1;
		m_pTransformCom->Turn({ 0,0,iOffsetRotation }, fTimeDelta);
	}
}

void CUI_LoadingMark::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_LoadingMark::Render(_float fTimeDelta)
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

HRESULT CUI_LoadingMark::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingMark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_LoadingMark* CUI_LoadingMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingMark* pInstatnce = new CUI_LoadingMark(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_LoadingMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_LoadingMark::Clone(void* pArg)
{
	CUI_LoadingMark* pInstatnce = new CUI_LoadingMark(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_LoadingMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_LoadingMark::Free()
{
	__super::Free();
}
