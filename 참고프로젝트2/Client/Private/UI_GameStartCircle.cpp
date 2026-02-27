#include "stdafx.h"

#include "UI_GameStartCircle.h"
#include "RenderInstance.h"

CUI_GameStartCircle::CUI_GameStartCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_GameStartCircle::CUI_GameStartCircle(const CUI_GameStartCircle& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_GameStartCircle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GameStartCircle::Initialize(void* pArg)
{
	m_fSizeX = 1000.f;
	m_fSizeY = 1000.f;
	m_fPosX = 640;
	m_fPosY = 360;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);

	m_iTextureIndex = pUI_Desc->iNumUI;
	
	Set_AnimPosition(600, 0.5f);
	Set_AnimPosition(600, 1.5f);  
	Set_AnimPosition(400, 1.6f);
	Set_AnimPosition(600, 1.7f);
	Set_AnimPosition(650, 1.75f);
	Set_AnimPosition(550, 1.8f);
	Set_AnimPosition(600, 1.9f);
	Set_AnimPosition(600, 2.f);
	Set_AnimPosition(600, 2.2f);
	Set_AnimPosition(900, 2.6f);

	m_iTotalAnimSize = m_QueueAnim.size();

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.9f);

	return S_OK;
}

void CUI_GameStartCircle::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	if(m_iTextureIndex == 0)
		m_pUI_Manager->m_fTotalDuration += fTimeDelta;
	//m_fTimer += fTimeDelta;

	int a = 10;
}

void CUI_GameStartCircle::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	//if (m_eAnimType == UI_ANIM)
	//{
		Action_Rotaion(fTimeDelta);
		Action_Anim(1.f, fTimeDelta);
	
		if(m_QueueAnim.empty())
			Destory();;
	
	//}
	
	if (m_bEmblem == FALSE && m_QueueAnim.size() == m_iTotalAnimSize - 1)
	{
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_StartEmblem"), TEXT("Layer_UI_Emblem"));
		m_bEmblem = TRUE;
	}

	//Action_Anim(1.f, fTimeDelta);

}

void CUI_GameStartCircle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_GameStartCircle::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GameStartCircle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameStartCircle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_GameStartCircle::Action_Rotaion(_float fTimeDelta)
{
	if (m_iTextureIndex != 0)
	{
		_float iOffsetRotation = 0;
		m_iTextureIndex % 2 == 0 ? iOffsetRotation = 1 : iOffsetRotation = -1;
		m_pTransformCom->Turn({ 0,0,iOffsetRotation }, fTimeDelta);
	}
}

CUI_GameStartCircle* CUI_GameStartCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_GameStartCircle* pInstatnce = new CUI_GameStartCircle(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_GameStartCircle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_GameStartCircle::Clone(void* pArg)
{
	CUI_GameStartCircle* pInstatnce = new CUI_GameStartCircle(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_GameStartCircle"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_GameStartCircle::Free()
{
	__super::Free();
}
