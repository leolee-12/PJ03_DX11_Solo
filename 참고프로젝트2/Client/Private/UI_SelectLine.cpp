#include "stdafx.h"
#include "..\Public\UI_SelectLine.h"

#include "GameInstance.h"
#include "RenderInstance.h"

#include "UI_Define.h"
#include "UI_SelectArrow.h";

CUI_SelectLine::CUI_SelectLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUI_SelectLine::CUI_SelectLine(const CUI_SelectLine& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CUI_SelectLine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_SelectLine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_LINE_DESC* Desc = static_cast<UI_LINE_DESC*>(pArg);
	m_iNumIndex = Desc->iNumChoice;

	InitPosition();
	m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(10.f));

	__super::Set_UI_Setting(60, 470, m_fPosX, 190, 0.f);

	return S_OK;
}

void CUI_SelectLine::Camera_Update(_float fTimeDelta)
{

}

void CUI_SelectLine::Update(_float fTimeDelta)
{

}

void CUI_SelectLine::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_SelectLine::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(34)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CUI_SelectLine::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterSelectLine"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_SelectLine::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

void CUI_SelectLine::InitPosition()
{
	switch (m_iNumIndex)
	{
	case 0:
		m_fPosX = 405;
		m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(10.f));
		break;

	case 1:
		m_fPosX = 265;
		m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(10.f));
		break;

	case 2:
		m_fPosX = 1280 - 405;
		m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(350.f));
		break;

	case 3:
		m_fPosX = 1280 - 245;
		m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(350.f));
		break;

	default:
		Destory();
		break;

	}
}

CUI_SelectLine* CUI_SelectLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SelectLine* pInstance = new CUI_SelectLine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_SelectLine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_SelectLine::Clone(void* pArg)
{
	CUI_SelectLine* pInstance = new CUI_SelectLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_SelectLine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_SelectLine::Free()
{
	__super::Free();
}
