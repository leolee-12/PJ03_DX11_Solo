#include "UIContainer.h"

CUIContainer::CUIContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIContainer::CUIContainer(const CUIContainer& Prototype)
	: CUIObject{ Prototype }
{
}

void CUIContainer::Set_Layout(const UILAYOUT_DESC& tLayoutDesc)
{
	m_tLayoutDesc = tLayoutDesc;
	Arrange_Children();
}

HRESULT CUIContainer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIContainer::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UICONTAINER_DESC*>(pArg);
		m_tLayoutDesc = pDesc->tLayoutDesc;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Arrange_Children();

	return S_OK;
}

void CUIContainer::Priority_Update(_float fTimeDelta)
{
}

void CUIContainer::Update(_float fTimeDelta)
{
}

void CUIContainer::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIContainer::Render()
{
	return S_OK;
}

void CUIContainer::Refresh_Layout()
{
	__super::Refresh_Layout();

	Arrange_Children();
}

void CUIContainer::Add_Child(CUIObject* pChild)
{
	if (nullptr == pChild || this == pChild)
		return;

	if (m_Children.end() != find(m_Children.begin(), m_Children.end(), pChild))
		return;

	if(auto pOldParent = dynamic_cast<CUIContainer*>(pChild->Get_ParentUI()))
		pOldParent->Remove_Child(pChild);

	pChild->Set_ParentUI(this);
	m_Children.push_back(pChild);
	Arrange_Children();
}

void CUIContainer::Remove_Child(CUIObject* pChild)
{
	auto iter = find(m_Children.begin(), m_Children.end(), pChild);
	if (m_Children.end() == iter)
		return;

	if (*iter)
		(*iter)->Set_ParentUI(nullptr);

	m_Children.erase(iter);

	Arrange_Children();
}

_bool CUIContainer::Insert_Child(_int iIndex, CUIObject* pChild)
{
	if (nullptr == pChild) return false;
	if (iIndex < 0 || iIndex > static_cast<_int>(m_Children.size())) return false;

	m_Children.insert(m_Children.begin() + iIndex, pChild);
	pChild->Set_ParentUI(this);
	Refresh_Layout();
	return true;
}

_bool CUIContainer::Move_Child(_int iFrom, _int iTo)
{
	const _int iSize = static_cast<_int>(m_Children.size());
	if (iFrom < 0 || iFrom >= iSize) return false;
	if (iTo < 0 || iTo >= iSize) return false;
	if (iFrom == iTo) return true;

	CUIObject* pMoved = m_Children[iFrom];
	m_Children.erase(m_Children.begin() + iFrom);
	m_Children.insert(m_Children.begin() + iTo, pMoved);
	Refresh_Layout();
	return true;
}

void CUIContainer::Arrange_Children()
{
	if (m_Children.empty())
		return;

	switch (m_tLayoutDesc.eLayout)
	{
	case UI_LAYOUT::NONE:
		break;

	case UI_LAYOUT::CANVAS:
		for (auto pChild : m_Children)
		{
			if (pChild)
				pChild->Refresh_Layout();
		}
		break;

	case UI_LAYOUT::OVERLAY:
		Arrange_Overlay();
		break;

	case UI_LAYOUT::HORIZONTAL:
		Arrange_Horizontal();
		break;

	case UI_LAYOUT::VERTICAL:
		Arrange_Vertical();
		break;

	default:
		break;
	}
}

void CUIContainer::Arrange_Overlay()
{
	const _float4 rc = Get_ScreenRect();
	const _float fCenterX = rc.x + rc.z * 0.5f;
	const _float fCenterY = rc.y + rc.w * 0.5f;

	for (auto pChild : m_Children)
	{
		if (pChild)
			pChild->Apply_LayoutCenter(fCenterX, fCenterY);
	}
}

void CUIContainer::Arrange_Horizontal()
{
	const _float4 rc = Get_ScreenRect();
	_float fCursor = rc.x + m_tLayoutDesc.fPadding;

	for (auto pChild : m_Children)
	{
		if (nullptr == pChild)
			continue;

		const auto& slot = pChild->Get_LayoutSlot();
		const _float2 vSize = pChild->Get_Size();

		const _float fChildW = slot.fDesiredSizeX > 0.f ? slot.fDesiredSizeX : vSize.x;
		const _float fChildH = slot.fDesiredSizeY > 0.f ? slot.fDesiredSizeY : vSize.y;

		fCursor += slot.vMargin.x;

		const _float fChildCenterX = fCursor + fChildW * 0.5f;
		const _float fChildCenterY =
			rc.y + rc.w * 0.5f + (slot.vMargin.y - slot.vMargin.w) * 0.5f;

		pChild->Apply_LayoutCenter(fChildCenterX, fChildCenterY);

		fCursor += fChildW + slot.vMargin.z + m_tLayoutDesc.fSpacing;
	}
}

void CUIContainer::Arrange_Vertical()
{
	const _float4 rc = Get_ScreenRect();
	_float fCursor = rc.y + m_tLayoutDesc.fPadding;

	for (auto pChild : m_Children)
	{
		if (nullptr == pChild)
			continue;

		const auto& slot = pChild->Get_LayoutSlot();
		const _float2 vSize = pChild->Get_Size();

		const _float fChildW = slot.fDesiredSizeX > 0.f ? slot.fDesiredSizeX : vSize.x;
		const _float fChildH = slot.fDesiredSizeY > 0.f ? slot.fDesiredSizeY : vSize.y;

		fCursor += slot.vMargin.y;

		const _float fChildCenterX =
			rc.x + rc.z * 0.5f + (slot.vMargin.x - slot.vMargin.z) * 0.5f;
		const _float fChildCenterY = fCursor + fChildH * 0.5f;

		pChild->Apply_LayoutCenter(fChildCenterX, fChildCenterY);

		fCursor += fChildH + slot.vMargin.w + m_tLayoutDesc.fSpacing;
	}
}

CUIContainer* CUIContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIContainer* pInstance = new CUIContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIContainer::Clone(void* pArg)
{
	CUIContainer* pInstance = new CUIContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIContainer::Free()
{
	__super::Free();

	m_Children.clear();
}