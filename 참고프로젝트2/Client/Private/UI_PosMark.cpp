#include "stdafx.h"

#include "UI_PosMark.h"
#include "RenderInstance.h"

#include "Character.h"

CUI_PosMark::CUI_PosMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_PosMark::CUI_PosMark(const CUI_PosMark& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_PosMark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PosMark::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 100.f, m_fSizeY = 100.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_PosMark::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_PosMark::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CUI_PosMark::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_PosMark::Render(_float fTimeDelta)
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

HRESULT CUI_PosMark::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufMark"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_PosMark::PlayerPositionCompare()
{
	CTransform* LPlayerTransform  = dynamic_cast<CTransform*>(m_pUI_Manager->m_pPawnArray[CUI_Define::LPLAYER1]->Get_Component(TEXT("Com_Transform")));
	CTransform* RPlayerTransform = dynamic_cast<CTransform*>(m_pUI_Manager->m_pPawnArray[CUI_Define::RPLAYER1]->Get_Component(TEXT("Com_Transform")));

	_float LPlayerPosY = XMVectorGetY(LPlayerTransform->Get_State(CTransform::STATE_POSITION));
	_float RPlayerPosY = XMVectorGetY(RPlayerTransform->Get_State(CTransform::STATE_POSITION));

	_float fHigherPlayerY = max(LPlayerPosY, RPlayerPosY);

	_float fPlayerDistanceY = fabs(LPlayerPosY - RPlayerPosY);




}

CUI_PosMark* CUI_PosMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PosMark* pInstatnce = new CUI_PosMark(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PosMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_PosMark::Clone(void* pArg)
{
	CUI_PosMark* pInstatnce = new CUI_PosMark(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_PosMark"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_PosMark::Free()
{
	__super::Free();
}
