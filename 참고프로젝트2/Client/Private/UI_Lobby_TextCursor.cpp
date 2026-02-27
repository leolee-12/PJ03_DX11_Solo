#include "stdafx.h"

#include "UI_Lobby_TextCursor.h"
#include "RenderInstance.h"

CUI_Lobby_TextCursor::CUI_Lobby_TextCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Lobby_TextCursor::CUI_Lobby_TextCursor(const CUI_Lobby_TextCursor& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Lobby_TextCursor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lobby_TextCursor::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 40.f, m_fSizeY = 40.f;
	m_fPosX = 1050.f, m_fPosY = 630.f;

	m_bIsActive = FALSE;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Lobby_TextCursor::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Lobby_TextCursor::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_bOffsetSwitch ? m_fOffsetPosY += fTimeDelta * 30.f : m_fOffsetPosY -= fTimeDelta * 20.f;

	if (m_fOffsetPosY >= 10.f)
		m_bOffsetSwitch = FALSE;
	if (m_fOffsetPosY <= 0.f)
		m_bOffsetSwitch = TRUE;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY , m_fPosX, m_fPosY + m_fOffsetPosY, 0.f);
}

void CUI_Lobby_TextCursor::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Lobby_TextCursor::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lobby_TextCursor::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_TextCursor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Lobby_TextCursor* CUI_Lobby_TextCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Lobby_TextCursor* pInstatnce = new CUI_Lobby_TextCursor(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Lobby_TextCursor"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Lobby_TextCursor::Clone(void* pArg)
{
	CUI_Lobby_TextCursor* pInstatnce = new CUI_Lobby_TextCursor(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Lobby_TextCursor"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Lobby_TextCursor::Free()
{
	__super::Free();
}
