#include "stdafx.h"

#include "UI_Chara_Icon_Panel.h"
#include "RenderInstance.h"

CUI_Chara_Icon_Panel::CUI_Chara_Icon_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Chara_Icon_Panel::CUI_Chara_Icon_Panel(const CUI_Chara_Icon_Panel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Chara_Icon_Panel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chara_Icon_Panel::Initialize(void* pArg)
{
	m_fSizeX = 84.f * 2.f;
	m_fSizeY = 84.f;
	m_fPosX = 73.f;
	m_fPosY = 78.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY,0.9f);

	return S_OK;
}

void CUI_Chara_Icon_Panel::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Chara_Icon_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Animation({ 72.f ,57.f ,0.85f, 1.f }, { m_fPosX, m_fPosY, 0.85f, 1.f }, 500.f, 0.85f, fTimeDelta);
}

void CUI_Chara_Icon_Panel::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Chara_Icon_Panel::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_vector vColor = { 1,1,1,1 };

	if(m_eLRPos == LEFT)
		vColor = { 1.f , 0.f , 0.f , 1.f };
	else if (m_eLRPos == RIGHT)
		vColor = { 0.f , 0.f , 1.f , 1.f };

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

HRESULT CUI_Chara_Icon_Panel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CharaIconPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Chara_Icon_Panel* CUI_Chara_Icon_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chara_Icon_Panel* pInstatnce = new CUI_Chara_Icon_Panel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Chara_Icon_Panel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Chara_Icon_Panel::Clone(void* pArg)
{
	CUI_Chara_Icon_Panel* pInstatnce = new CUI_Chara_Icon_Panel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Chara_Icon_Panel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Chara_Icon_Panel::Free()
{
	__super::Free();
}
