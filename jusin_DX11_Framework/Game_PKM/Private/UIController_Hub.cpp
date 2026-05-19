#include "UIController_Hub.h"
#include "UIController.h"
#include "UIButton_Group.h"

#include "UISequence.h"
#include "UIButton.h"

CUIController_Hub::CUIController_Hub()
{
}

HRESULT CUIController_Hub::Initialize()
{
	return S_OK;
}

HRESULT CUIController_Hub::Register(CUIController* pCtrl, _uint iOwnerLevel)
{
	if (nullptr == pCtrl)
		return E_FAIL;

	for (const CONTROLLER_ENTRY& Entry : m_Controllers)
	{
		if (Entry.pCtrl == pCtrl)
			return S_FALSE;
	}

	Safe_AddRef(pCtrl);

	CONTROLLER_ENTRY Entry{};
	Entry.iOwnerLevel = iOwnerLevel;
	Entry.pCtrl = pCtrl;
	m_Controllers.push_back(Entry);

	return S_OK;
}

void CUIController_Hub::Unregister(CUIController* pCtrl)
{
	if (nullptr == pCtrl)
		return;

	for (auto it = m_Controllers.begin(); it != m_Controllers.end(); ++it)
	{
		if (it->pCtrl == pCtrl)
		{
			if (m_pLastActiveCtrl == pCtrl)
				m_pLastActiveCtrl = nullptr;

			CUIController* pRelease = it->pCtrl;
			m_Controllers.erase(it);
			Safe_Release(pRelease);
			return;
		}
	}
}

void CUIController_Hub::Update_All(_float fTimeDelta)
{	/* 모든 등록 컨트롤러에 Update 전파 (iterator 무효화 주의 - Unregister/Close_All) */
	for (CONTROLLER_ENTRY& Entry : m_Controllers)
	{
		CUIController* pCtrl = Entry.pCtrl;

		if (nullptr != pCtrl)
			pCtrl->Update(fTimeDelta);
	}

	Update_Cursor(fTimeDelta);
}

void CUIController_Hub::Close_All()
{
	m_pLastActiveCtrl = nullptr;

	for (CONTROLLER_ENTRY& Entry : m_Controllers)
		Safe_Release(Entry.pCtrl);

	m_Controllers.clear();
}

void CUIController_Hub::Cleanup_Level(_uint iOwnerLevel)
{
	for (auto it = m_Controllers.begin(); it != m_Controllers.end();)
	{
		if (it->iOwnerLevel != iOwnerLevel)
		{
			++it;
			continue;
		}

		CUIController* pRelease = it->pCtrl;

		if (m_pLastActiveCtrl == pRelease)
			m_pLastActiveCtrl = nullptr;

		it = m_Controllers.erase(it);
		Safe_Release(pRelease);
	}
}

_bool CUIController_Hub::Is_AnyOpen() const
{
	for (const CONTROLLER_ENTRY& Entry : m_Controllers)
	{
		CUIController* pCtrl = Entry.pCtrl;

		if (nullptr != pCtrl && pCtrl->Is_Open())
			return true;
	}

	return false;
}

void CUIController_Hub::Set_Cursor_Sequence(CUISequence* pSeq)
{
	m_pCursor = pSeq;  // weak

	if (nullptr == m_pCursor)
		return;

	/* 시퀀스 root layout 을 CANVAS 로 강제.
	   이유: Build_FromFile 은 시퀀스 root 의 layout 을 파일에서 읽지 않아 기본값 NONE 으로 들어옴.
			NONE 이면 Arrange_Children 이 자식 Refresh_Layout 을 호출하지 않아,
			시퀀스 Set_Center 후에도 자식 widget_001 의 anchor MC 재계산이 일어나지 않음. */
	UILAYOUT_DESC tLayout{};
	tLayout.eLayout = UI_LAYOUT::CANVAS;
	m_pCursor->Set_Layout(tLayout);

	/* 초기 상태는 숨김. 활성 컨트롤러가 발생하면 Update_Cursor 가 표시 처리. */
	m_pCursor->Set_Visible(false);

	/* 좌우 흔들기 트윈은 timeline 의 PLAY_ANIM 으로 발화되므로 한 번만 Play 호출.
	   시퀀스 자체의 매 프레임 Update 는 레벨이 게임 객체로 등록해 진행한다 (유닛 ③). */
	m_pCursor->Play();
}

CUIController* CUIController_Hub::Find_Active_Controller() const
{
	/* m_Controllers 뒤에서부터 순회 -> 가장 마지막에 Register 된 활성 컨트롤러 우선.
	   활성 조건: Open && 그룹 Active && 그룹에 포커스 버튼 존재. */
	for (auto it = m_Controllers.rbegin(); it != m_Controllers.rend(); ++it)
	{
		CUIController* pCtrl = it->pCtrl;

		if (nullptr == pCtrl)
			continue;
		if (false == pCtrl->Is_Open())
			continue;

		CUIButton_Group* pGroup = pCtrl->Get_Group();
		if (nullptr == pGroup || false == pGroup->Is_Active())
			continue;
		if (nullptr == pGroup->Get_FocusedButton())
			continue;

		return pCtrl;
	}
	return nullptr;
}

void CUIController_Hub::Update_Cursor(_float fTimeDelta)
{
	if (nullptr == m_pCursor)
		return;

	CUIController* pActive = Find_Active_Controller();

	/* 활성 컨트롤러가 이전 프레임과 달라지면 딜레이 타이머 리셋.
	   (nullptr -> 신규 / 신규 -> 다른 컨트롤러 모두 포함.
		같은 컨트롤러 내부 포커스 이동은 이 분기에 걸리지 않음.) */
	if (pActive != m_pLastActiveCtrl)
	{
		m_fCursorShowTimer = m_fCursorShowDelay;
		m_pLastActiveCtrl = pActive;
	}

	if (nullptr == pActive)
	{
		m_pCursor->Set_Visible(false);
		return;
	}

	/* 딜레이 진행 중 - 매 프레임 감소시키되 커서는 숨김 유지. */
	if (m_fCursorShowTimer > 0.f)
	{
		m_fCursorShowTimer -= fTimeDelta;
		m_pCursor->Set_Visible(false);
		return;
	}

	CUIButton* pFocused = pActive->Get_Group()->Get_FocusedButton();
	if (nullptr == pFocused)
	{
		m_pCursor->Set_Visible(false);
		return;
	}

	/* 버튼의 디자인 사각형(left, top, width, height) 기준으로
	   "좌측 가장자리의 세로 중앙" 에 커서 시퀀스를 정렬.
	   좌우 흔들기(ANCHOR_OFFSET_X 트윈)는 이 기준점 주변으로 동작. */
	const _float4 vRect = pFocused->Get_DesignRect();
	const _float fAnchorX = vRect.x;
	const _float fAnchorY = vRect.y + vRect.w * 0.5f;
	m_pCursor->Set_Center(fAnchorX, fAnchorY);
	m_pCursor->Set_Visible(true);
}

CUIController_Hub* CUIController_Hub::Create()
{
	CUIController_Hub* pInstance = new CUIController_Hub();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CUIController_Hub");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIController_Hub::Free()
{
	m_pCursor = nullptr;          // weak
	m_pLastActiveCtrl = nullptr;  // weak (비교용)
	Close_All();

	__super::Free();
}