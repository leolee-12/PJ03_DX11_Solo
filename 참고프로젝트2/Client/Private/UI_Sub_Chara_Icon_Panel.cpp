#include "stdafx.h"

#include "UI_Sub_Chara_Icon_Panel.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_Sub_Chara_Icon_Panel::CUI_Sub_Chara_Icon_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Sub_Chara_Icon_Panel::CUI_Sub_Chara_Icon_Panel(const CUI_Sub_Chara_Icon_Panel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Sub_Chara_Icon_Panel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Sub_Chara_Icon_Panel::Initialize(void* pArg)
{
	m_fPosX = 83.f;
	m_fPosY = 105.f;
	m_fSizeX = 184.f;
	m_fSizeY = 46.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_UI_Setting(-m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.65f);

	return S_OK;
}

void CUI_Sub_Chara_Icon_Panel::Camera_Update(_float fTimeDelta)
{

}

void CUI_Sub_Chara_Icon_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	Animation({ 73.f ,78.f ,0.65f, 1.f }, { m_fPosX, m_fPosY, 0.65f, 1.f }, 500.f, 0.65f, fTimeDelta);

	if (m_pSubPawn == nullptr)
		Destory();
}

void CUI_Sub_Chara_Icon_Panel::Late_Update(_float fTimeDelta)
{
	
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
	
}

HRESULT CUI_Sub_Chara_Icon_Panel::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iPass = 0;
	_vector vColor = { 0.35f ,0.35f ,0.35f };
	if (m_pSubPawn != nullptr)
	{
		_float fHpRadio = m_pSubPawn->Get_PawnDesc().iHp / 10000.f;
		
		if (fHpRadio > 0.f)
			iPass = 0;
		else
		{
			iPass = 2;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector))))
				return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Begin(iPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Sub_Chara_Icon_Panel::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SubCharaIconPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Sub_Chara_Icon_Panel::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	_uint  iTexIdx = { 0 };

	if (m_eLRPos == LEFT)
		iTexIdx = 1;
	else if (m_eLRPos == RIGHT)
		iTexIdx = 0;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iTexIdx)))
		return E_FAIL;

	return S_OK;
}

CUI_Sub_Chara_Icon_Panel* CUI_Sub_Chara_Icon_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Sub_Chara_Icon_Panel* pInstatnce = new CUI_Sub_Chara_Icon_Panel(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Sub_Chara_Icon_Panel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Sub_Chara_Icon_Panel::Clone(void* pArg)
{
	CUI_Sub_Chara_Icon_Panel* pInstatnce = new CUI_Sub_Chara_Icon_Panel(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Sub_Chara_Icon_Panel"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Sub_Chara_Icon_Panel::Free()
{

	__super::Free();
}
