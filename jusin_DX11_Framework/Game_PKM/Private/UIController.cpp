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

	if (FAILED(Resolve_Buttons()))
		return E_FAIL;

	if (FAILED(Build_Group()))
		return E_FAIL;

	if (nullptr == m_pGroup)
		return E_FAIL;

	/* 시작은 닫힘. Open() 호출 전까지 입력 무시 */
	m_pGroup->Set_Active(false);
	m_bOpen = false;

	return S_OK;
}

void CUIController::Update(_float fTimeDelta)
{
	if (false == m_bOpen)
		return;

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
	if (nullptr == m_pGroup)
		return;

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

	if (nullptr != m_pSequence)
	{
		m_pSequence->Set_Visible(true);
		m_pSequence->Play();
	}

	m_pGroup->Set_Active(true);
	m_bOpen = true;
}

void CUIController::Open(_bool bForceReset)
{
	if (nullptr == m_pGroup)
		return;


	if (bForceReset)
		m_pGroup->Set_FocusedIndex(0);
	else
		m_pGroup->Set_FocusedIndex(m_iLastFocusedIndex);

	if (nullptr != m_pSequence)
	{
		m_pSequence->Set_Visible(true);
		m_pSequence->Play();
	}

	m_pGroup->Set_Active(true);
	m_bOpen = true;
}

void CUIController::Close()
{
	if (nullptr == m_pGroup)
		return;


	/* 정책 무관 — 마지막 포커스는 항상 저장 */
	m_iLastFocusedIndex = m_pGroup->Get_FocusedIndex();

	if (nullptr != m_pSequence)
	{
		for (auto* p : m_pSequence->Get_Children())
			if (nullptr != p) p->Set_Visible(false);

		m_pSequence->Set_Visible(false);
		m_pSequence->Stop();
	}

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

void CUIController::Free()
{
	__super::Free();

	m_pSequence = nullptr;  // weak
	Safe_Release(m_pGroup);
	Safe_Release(m_pGameInstance);
}