#include "stdafx.h"

#include "UI_VS_NameOutLine.h"
#include "RenderInstance.h"

#include "BattleInterface.h"

CUI_VS_NameOutLine::CUI_VS_NameOutLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_VS_NameOutLine::CUI_VS_NameOutLine(const CUI_VS_NameOutLine& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_VS_NameOutLine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_NameOutLine::Initialize(void* pArg)
{
	m_fPosX = 205.f, m_fPosY = 260.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 350.f , m_fSizeY = 45.f ;

	UI_DESC* Desc = static_cast<UI_DESC*>(pArg);
	m_fTotalNum = Desc->iTotalNum;
	m_iNumUI = Desc->iNumUI;
	_uint eLR = Desc->eLRPos;

	m_pTransformCom->Rotation({ 0.f , 0.f, 1.f }, XMConvertToRadians(-5.f));

	if (eLR == RIGHT)
		m_pTransformCom->Rotation({ 0.f , 0.f, 1.f }, XMConvertToRadians(5.f));

	if (m_iNumUI == 1)
		m_fPosY += 230;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_VS_NameOutLine::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_VS_NameOutLine::Update(_float fTimeDelta)
{
	if (fTimeDelta >= 0.1f)
		fTimeDelta = 0.f;

	__super::Update(fTimeDelta);

	if (m_iNumUI == 0)
		Animation({ 0.f, m_fPosY - 200.f  ,0.f, 1.f }, { m_fPosX, m_fPosY, 0.f, 1.f }, 2100.f, 0.f, fTimeDelta);
	else if (m_iNumUI == 1)
		Animation({ -500, m_fPosY - 200.f  ,0.f, 1.f }, { m_fPosX, m_fPosY, 0.f, 1.f }, 2700.f, 0.f, fTimeDelta);
}

void CUI_VS_NameOutLine::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_VS_NameOutLine::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	CUI_Define::PLAYER_ID ePlayerID = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(m_fTotalNum).ePlayerID;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", ePlayerID)))
		return E_FAIL;

	_vector vColor = { 0.f, 0.f,  0.f ,1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &(vColor), sizeof(_vector))))
		return E_FAIL;
	
	_float fAlphaValue = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &(fAlphaValue), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(13)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_VS_NameOutLine::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_FontName"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUI_VS_NameOutLine* CUI_VS_NameOutLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_VS_NameOutLine* pInstatnce = new CUI_VS_NameOutLine(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_VS_NameOutLine"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_VS_NameOutLine::Clone(void* pArg)
{
	CUI_VS_NameOutLine* pInstatnce = new CUI_VS_NameOutLine(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_VS_NameOutLine"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_VS_NameOutLine::Free()
{
	__super::Free();
}
