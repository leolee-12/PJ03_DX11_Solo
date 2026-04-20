#include "UIProgressBar.h"

CUIProgressBar::CUIProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIProgressBar::CUIProgressBar(const CUIProgressBar& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIProgressBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIProgressBar::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UIPROGRESSBAR_DESC*>(pArg);
		m_strBackTextureTag = pDesc->strBackTextureTag;
		m_iBackTextureIndex = pDesc->iBackTextureIndex;
		m_strFillTextureTag = pDesc->strFillTextureTag;
		m_iFillTextureIndex = pDesc->iFillTextureIndex;
		m_eDirection = pDesc->eDirection;
		Set_FillAmount(pDesc->fFillAmount);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIProgressBar::Priority_Update(_float fTimeDelta)
{
}

void CUIProgressBar::Update(_float fTimeDelta)
{
}

void CUIProgressBar::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIProgressBar::Render()
{
	return S_OK;
}

CUIProgressBar* CUIProgressBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIProgressBar* pInstance = new CUIProgressBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIProgressBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIProgressBar::Clone(void* pArg)
{
	CUIProgressBar* pInstance = new CUIProgressBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIProgressBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIProgressBar::Free()
{
	__super::Free();
}