#include "UIText.h"

CUIText::CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIText::CUIText(const CUIText& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIText::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UITEXT_DESC*>(pArg);
		m_strText = pDesc->strText;
		m_strFontTag = pDesc->strFontTag;
		m_vColor = pDesc->vColor;
		m_eAlign = pDesc->eAlign;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIText::Priority_Update(_float fTimeDelta)
{
}

void CUIText::Update(_float fTimeDelta)
{
}

void CUIText::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIText::Render()
{
	return S_OK;
}

CUIText* CUIText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIText* pInstance = new CUIText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIText::Clone(void* pArg)
{
	CUIText* pInstance = new CUIText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIText::Free()
{
	__super::Free();
}