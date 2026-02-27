#include "stdafx.h"

#include "UI_KOFont.h"
#include "RenderInstance.h"

CUI_KOFont::CUI_KOFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_GameState{ pDevice ,pContext }
{
}

CUI_KOFont::CUI_KOFont(const CUI_KOFont& Prototype)
	:CUI_GameState{ Prototype }
{
}

HRESULT CUI_KOFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_KOFont::Initialize(void* pArg)
{
	m_fSizeX = 600.f;
	m_fSizeY = 600.f;
	m_fPosX = 640;
	m_fPosY = 360;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pUI_Desc = static_cast<UI_DESC*>(pArg);
	

	Set_AnimPosition(400, 0.5f);
	Set_AnimPosition(300, 0.55f);
	Set_AnimPosition(350, 0.65f);
	Set_AnimPosition(700, 0.75f);
	Set_AnimPosition(800, 1.f);

	m_fAlphaValue = 1.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY , 0.f);

	return S_OK;
}

void CUI_KOFont::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_pUI_Manager->m_fTotalDuration += fTimeDelta;

	if(m_QueueAnim.size() <= 1)
		m_fAlphaValue -= fTimeDelta * 4.f;

	if (m_fAlphaValue <= 0.f)
		m_fAlphaValue = 0.f;


}

void CUI_KOFont::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_QueueAnim.empty())
		Destory();

	Action_Anim(1.f, fTimeDelta);
}

void CUI_KOFont::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_KOFont::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(15)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_KOFont::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameKOFont"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_KOFont::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	//1번이 BG(노란색) 2번이 Main(검정색)
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CUI_KOFont* CUI_KOFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_KOFont* pInstatnce = new CUI_KOFont(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_KOFont"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_KOFont::Clone(void* pArg)
{
	CUI_KOFont* pInstatnce = new CUI_KOFont(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_KOFont"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_KOFont::Free()
{

	__super::Free();
}
