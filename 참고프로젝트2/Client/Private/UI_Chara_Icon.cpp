#include "stdafx.h"

#include "UI_Chara_Icon.h"
#include "RenderInstance.h"
#include "Character.h"

CUI_Chara_Icon::CUI_Chara_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Chara_Icon::CUI_Chara_Icon(const CUI_Chara_Icon& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Chara_Icon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chara_Icon::Initialize(void* pArg)
{
	m_fPosX = 50.f;
	m_fPosY = 20.f;
	m_fSizeY = 150.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 150.f;
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY ,0.85f);

	return S_OK;
}

void CUI_Chara_Icon::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	
}

void CUI_Chara_Icon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pMainPawn != nullptr)
		m_iCharaID = m_pMainPawn->Get_PawnDesc().ePlayerID;
	
	Animation({ 72.f ,100.f ,0.85f, 1.f }, { m_fPosX, m_fPosY, 0.85f, 1.f }, 500.f, 0.85f, fTimeDelta);
}

void CUI_Chara_Icon::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Chara_Icon::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;
	 
	//m_iCharaID → 캐릭터 ID가 만들어주면 0대신에 넣어주는 값 (캐릭터에 따라서 이미지가 바뀌도록)
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCharaID)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chara_Icon::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CharaIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Chara_Icon* CUI_Chara_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chara_Icon* pInstatnce = new CUI_Chara_Icon(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Chara_Icon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Chara_Icon::Clone(void* pArg)
{
	CUI_Chara_Icon* pInstatnce = new CUI_Chara_Icon(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Chara_Icon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Chara_Icon::Free()
{
	__super::Free();
}
