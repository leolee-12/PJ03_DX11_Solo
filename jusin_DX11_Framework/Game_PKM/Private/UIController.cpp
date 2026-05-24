#include "UIController.h"
#include "GameInstance.h"
#include "UISequence.h"

CUIController::CUIController()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIController::Initialize(CUISequence* pSequence)
{
	if (nullptr == pSequence)
		return E_FAIL;

	m_pSequence = pSequence;  // weak

	if (FAILED(Resolve_Widgets()))
		return E_FAIL;

	if (FAILED(Resolve_Buttons()))
		return E_FAIL;

	if (FAILED(Build_Group()))
		return E_FAIL;

	if (FAILED(Bind_Sequence_Slots()))
		return E_FAIL;

	// 닫힌 채로 시작 (Open 전까지 입력 무시)
	if (nullptr != m_pGroup)
		m_pGroup->Set_Active(false);

	m_bOpen = false;

	return S_OK;
}

void CUIController::Update(_float fTimeDelta)
{
	if (false == m_bOpen)
		return;

	On_Update(fTimeDelta);

	if (nullptr == m_pGroup)
		return;

	m_pGroup->Update(fTimeDelta);

	/* 활성화 신호: 콜백만 호출하고 닫지 않음
	   (어떤 항목은 다른 패널을 열고 메뉴는 그대로 두고 싶을 수 있음) */
	if (m_pGroup->Was_Activated_This_Frame())
	{
		if (m_fnOnActivate)
			m_fnOnActivate(m_pGroup->Get_Activated_Index());
	}

	/* 취소 신호: 콜백 호출 후 자동으로 Close() */
	if (m_pGroup->Was_Cancelled_This_Frame())
	{
		if (m_fnOnCancel)
			m_fnOnCancel();
		Close();
	}
}

void CUIController::Open()
{
	if (nullptr != m_pGroup)
	{
		switch (m_eFocusPolicy)
		{
		case FOCUS_POLICY::RESET_ON_OPEN:
			m_pGroup->Set_FocusedIndex(0);
			break;
		case FOCUS_POLICY::REMEMBER_LAST:
			m_pGroup->Set_FocusedIndex(m_iLastFocusedIndex);
			break;
		default:
			break;
		}
	}

	if (nullptr != m_pSequence)
	{
		m_pSequence->Set_Visible(true);
		m_pSequence->Play();
	}

	if (nullptr != m_pGroup)
		m_pGroup->Set_Active(true);

	m_bOpen = true;
}

void CUIController::Open(_bool bForceReset)
{
	if (nullptr != m_pGroup)
	{
		if (bForceReset)
			m_pGroup->Set_FocusedIndex(0);
		else
			m_pGroup->Set_FocusedIndex(m_iLastFocusedIndex);
	}

	if (nullptr != m_pSequence)
	{
		m_pSequence->Set_Visible(true);
		m_pSequence->Play();
	}

	if (nullptr != m_pGroup)
		m_pGroup->Set_Active(true);

	m_bOpen = true;
}

void CUIController::Close()
{
	if (nullptr != m_pGroup)
		m_iLastFocusedIndex = m_pGroup->Get_FocusedIndex();

	if (nullptr != m_pSequence)
	{
		for (auto* p : m_pSequence->Get_Children())
			if (nullptr != p) p->Set_Visible(false);

		m_pSequence->Set_Visible(false);
		m_pSequence->Stop();
	}

	if (nullptr != m_pGroup)
		m_pGroup->Set_Active(false);

	m_bOpen = false;
}

void CUIController::Set_FocusPolicy(FOCUS_POLICY ePolicy)
{
	m_eFocusPolicy = ePolicy;
}

void CUIController::Set_OnActivate(ACTIVATE_CALLBACK fn)
{
	m_fnOnActivate = fn;
}

void CUIController::Set_OnCancel(CANCEL_CALLBACK fn)
{
	m_fnOnCancel = fn;
}
 
void CUIController::Set_KeyBinding(CUIButton_Group::NAVKEY eNav, _ubyte byDIK)
{
	if (nullptr == m_pGroup)
		return;
	m_pGroup->Set_KeyBinding(eNav, byDIK);
}

void CUIController::Hide_AllContents()
{
	if (nullptr == m_pSequence || true == m_bContentsHidden)
		return;

	m_VisibilitySnapshot.clear();

	vector<CUIObject*> Widgets;
	Collect_Widgets(m_pSequence, Widgets);

	for (CUIObject* pWidget : Widgets)
	{
		if (nullptr == pWidget)
			continue;

		m_VisibilitySnapshot.emplace_back(pWidget, pWidget->Get_Visible());
		pWidget->Set_Visible(false);
	}

	m_bContentsHidden = true;
}

void CUIController::Show_AllContents()
{
	if (false == m_bContentsHidden)
		return;

	for (auto& Pair : m_VisibilitySnapshot)
	{
		if (nullptr != Pair.first)
			Pair.first->Set_Visible(Pair.second);
	}

	m_VisibilitySnapshot.clear();
	m_bContentsHidden = false;
}

void CUIController::Collect_Widgets(CUIObject* pRoot, vector<CUIObject*>& out) const
{
	CUIContainer* pContainer = dynamic_cast<CUIContainer*>(pRoot);
	if (nullptr == pContainer)
		return;

	// 위젯이 렌더러에 개별 등록(플랫)되므로, 루트만 토글하면 중첩 위젯이 남는다.
	// 트리를 재귀로 훑어 모든 위젯을 수집한다.
	for (CUIObject* pChild : pContainer->Get_Children())
	{
		if (nullptr == pChild)
			continue;

		out.push_back(pChild);
		Collect_Widgets(pChild, out);
	}
}

void CUIController::Free()
{
	__super::Free();

	m_pSequence = nullptr;  // weak
	Safe_Release(m_pGroup);
	Safe_Release(m_pGameInstance);
}