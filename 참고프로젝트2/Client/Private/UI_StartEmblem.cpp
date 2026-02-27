#include "stdafx.h"

#include "UI_StartEmblem.h"
#include "RenderInstance.h"

CUI_StartEmblem::CUI_StartEmblem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_StartEmblem::CUI_StartEmblem(const CUI_StartEmblem& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_StartEmblem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StartEmblem::Initialize(void* pArg)
{

	m_fSizeX = 800.f;
	m_fSizeY = 800.f;
	m_fPosX = 640;
	m_fPosY = 360;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);
	Set_AnimPosition(300, 1.5f);
	Set_AnimPosition(250, 1.6f);
	Set_AnimPosition(300, 1.7f);
	Set_AnimPosition(350, 1.75f);
	Set_AnimPosition(250, 1.8f);
	Set_AnimPosition(200, 1.9f);
	Set_AnimPosition(200, 2.f);
	Set_AnimPosition(200, 2.2f);
	Set_AnimPosition(500, 2.6f);


	__super::Set_UI_Setting(m_fSizeX , m_fSizeY, m_fPosX, m_fPosY , 0.9f);

	return S_OK;
}

void CUI_StartEmblem::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);


}

void CUI_StartEmblem::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Action_Anim(1.f, fTimeDelta);


}

void CUI_StartEmblem::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_QueueAnim.empty())
		Destory();

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_StartEmblem::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float fTemp = 0.25f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &(fTemp), sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StartEmblem::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameStartEmblem"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CUI_StartEmblem* CUI_StartEmblem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StartEmblem* pInstatnce = new CUI_StartEmblem(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_StartEmblem"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_StartEmblem::Clone(void* pArg)
{
	CUI_StartEmblem* pInstatnce = new CUI_StartEmblem(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_StartEmblem"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_StartEmblem::Free()
{
	__super::Free();
}