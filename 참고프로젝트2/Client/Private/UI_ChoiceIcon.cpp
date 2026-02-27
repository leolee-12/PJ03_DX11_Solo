#include "stdafx.h"

#include "UI_ChoiceIcon.h"
#include "UI_SelectArrow.h"
#include "RenderInstance.h"

CUI_ChoiceIcon::CUI_ChoiceIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_ChoiceIcon::CUI_ChoiceIcon(const CUI_ChoiceIcon& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_ChoiceIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ChoiceIcon::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_CHOICE_DESC* pDesc = static_cast<UI_CHOICE_DESC*>(pArg);
	m_iTextrueIndex  = pDesc->iTextrueIndex;
	_vector vPos = pDesc->vInitPosition;
	_uint iTeam = pDesc->iTeam;

	_float fOffSetX = { 0 };

	switch (iTeam)
	{
	case 0:
		fOffSetX = -30;
		break;
	case 1:
		fOffSetX = 30;
		break;
	default:
		break;
	}


	_float fPosX = XMVectorGetX(vPos) + fOffSetX;

	m_fSizeX = 50;
	m_fSizeY = 50;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, fPosX, 600, 0.0f);

	return S_OK;
}

void CUI_ChoiceIcon::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_ChoiceIcon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CUI_ChoiceIcon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_ChoiceIcon::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextrueIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ChoiceIcon::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterChoiceMark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_ChoiceIcon* CUI_ChoiceIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ChoiceIcon* pInstatnce = new CUI_ChoiceIcon(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_ChoiceIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_ChoiceIcon::Clone(void* pArg)
{
	CUI_ChoiceIcon* pInstatnce = new CUI_ChoiceIcon(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_ChoiceIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_ChoiceIcon::Free()
{

	__super::Free();
}
