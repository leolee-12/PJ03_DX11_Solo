#include "Battle_InputDirector.h"
#include "Battle_Manager.h"
#include "Battle_CommandMenu.h"
#include "Battle_MoveMenu.h"
#include "Entry.h"
#include "Player_Status.h"
#include "Battle_EventDispatcher.h"
#include "CommandQueue.h"
#include "IBattleCommand.h"
#include "Battle_Commands.h"
#include "Battler.h"

CBattle_InputDirector::CBattle_InputDirector()
{
}

HRESULT CBattle_InputDirector::Initialize()
{
	return S_OK;
}

void CBattle_InputDirector::Bind(CBattle_Manager* pManager,
	CBattle_CommandMenu* pCommandMenu,
	CBattle_MoveMenu* pMoveMenu,
	CEntry* pEntry)
{
	m_pManager = pManager;
	m_pCommandMenu = pCommandMenu;
	m_pMoveMenu = pMoveMenu;
	m_pEntry = pEntry;

	// 콜백 등록 - 람다에서 this 캡처. Director 생명주기가 메뉴보다 같거나 짧음을 보장 (Level 이 모두 정리)
	if (nullptr != m_pCommandMenu)
	{
		m_pCommandMenu->Set_OnActivate([this](_int iIdx) { this->Handle_CommandActivate(iIdx); });
		m_pCommandMenu->Set_OnCancel([this]() { this->Handle_CommandCancel(); });
	}

	if (nullptr != m_pMoveMenu)
	{
		m_pMoveMenu->Set_OnActivate([this](_int iIdx) { this->Handle_MoveActivate(iIdx); });
		m_pMoveMenu->Set_OnCancel([this]() { this->Handle_MoveCancel(); });
	}

	if (nullptr != m_pEntry)
	{
		m_pEntry->Set_OnActivate([this](_int iIdx) { this->Handle_EntryActivate(iIdx); });
		m_pEntry->Set_OnCancel([this]() { this->Handle_EntryCancel(); });
	}

	// 초기 상태: 두 메뉴 비활성
	Enter_Idle();
}

void CBattle_InputDirector::Tick(_float fTimeDelta)
{
	(void)fTimeDelta;

	if (false == m_bModeChangePending)
		return;

	m_bModeChangePending = false;
	m_eMode = m_ePendingMode;

	switch (m_eMode)
	{
	case MODE::MAIN:
		if (nullptr != m_pCommandMenu)
			m_pCommandMenu->Open();
		break;

	case MODE::MOVE:
		if (nullptr != m_pMoveMenu)
			m_pMoveMenu->Open();
		break;

	case MODE::ENTRY:
		if (nullptr != m_pEntry)
			m_pEntry->Open();
		break;

	case MODE::IDLE:
	default:
		break;
	}
}

void CBattle_InputDirector::On_TurnStarted(const EVENT_TURN_STARTED& tEvent)
{
	(void)tEvent;
	Enter_Main();
}

void CBattle_InputDirector::Handle_CommandActivate(_int iIndex)
{
	using COMMAND = CBattle_CommandMenu::COMMAND;
	const COMMAND eCmd = static_cast<COMMAND>(iIndex);

	switch (eCmd)
	{
	case COMMAND::FIGHT:
		Enter_Move();
		break;

	case COMMAND::POKE:
		Enter_Entry();
		break;

	case COMMAND::BAG:
		// 결정 2B: 무반응 + 메뉴 재오픈. 현재 메뉴 그대로 유지.
		break;

	default:
		break;
	}
}

void CBattle_InputDirector::Handle_CommandCancel()
{
	// 결정 1B: MAIN 에서 Cancel -> 도망
	Submit_Run();
}

void CBattle_InputDirector::Handle_MoveActivate(_int iIndex)
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(CBattle_MoveMenu::SLOT::END))
		return;

	const _uint iSlot = static_cast<_uint>(iIndex);

	// PP 0 / 빈 슬롯은 무시 (MOVE 모드 유지)
	if (nullptr != m_pManager)
	{
		CBattler* pPlayer = m_pManager->Get_Battler(g_kBattleSide_Player);
		if (nullptr != pPlayer)
		{
			const _uint iMoveID = pPlayer->Get_MoveID(iSlot);
			const _ubyte iPP = pPlayer->Get_PP(iSlot);

			if (0 == iMoveID || 0 == iPP)
				return;
		}
	}

	Submit_Move(iSlot);
}

void CBattle_InputDirector::Handle_MoveCancel()
{
	Enter_Main();
}

void CBattle_InputDirector::Handle_EntryActivate(_int iIndex)
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(g_kMaxPartySize))
		return;

	Submit_Switch(static_cast<_uint>(iIndex));
}

void CBattle_InputDirector::Handle_EntryCancel()
{
	Enter_Main();
}

void CBattle_InputDirector::Enter_Main()
{
	if (nullptr != m_pMoveMenu)
		m_pMoveMenu->Close();

	if (nullptr != m_pEntry)
		m_pEntry->Close();

	if (nullptr != m_pCommandMenu)
		m_pCommandMenu->Close();

	m_ePendingMode = MODE::MAIN;
	m_bModeChangePending = true;
}

void CBattle_InputDirector::Enter_Move()
{
	if (nullptr != m_pCommandMenu)
		m_pCommandMenu->Close();

	if (nullptr != m_pEntry)
		m_pEntry->Close();

	if (nullptr != m_pMoveMenu)
		m_pMoveMenu->Close();

	m_ePendingMode = MODE::MOVE;
	m_bModeChangePending = true;
}

void CBattle_InputDirector::Enter_Entry()
{
	if (nullptr != m_pCommandMenu)
		m_pCommandMenu->Close();

	if (nullptr != m_pMoveMenu)
		m_pMoveMenu->Close();

	if (nullptr != m_pEntry)
		m_pEntry->Close();

	m_ePendingMode = MODE::ENTRY;
	m_bModeChangePending = true;
}


void CBattle_InputDirector::Enter_Idle()
{
	if (nullptr != m_pCommandMenu)
		m_pCommandMenu->Close();

	if (nullptr != m_pMoveMenu)
		m_pMoveMenu->Close();

	if (nullptr != m_pEntry)
		m_pEntry->Close();

	m_eMode = MODE::IDLE;
	m_bModeChangePending = false;  // Idle 은 즉시 - 다음 Tick 에 Open 할 메뉴 없음
}

HRESULT CBattle_InputDirector::Submit_Move(_uint iMoveSlot)
{
	if (nullptr == m_pManager)
		return E_FAIL;

	CCommandQueue* pQueue = m_pManager->Get_Queue();
	if (nullptr == pQueue)
		return E_FAIL;

	CMoveCommand::DESC tDesc{};
	tDesc.iActorSide = g_kBattleSide_Player;
	tDesc.iActorSlot = 0;
	tDesc.iMoveSlot = iMoveSlot;
	tDesc.iTargetSide = g_kBattleSide_Opponent;
	tDesc.iTargetSlot = 0;

	IBattleCommand* pCommand = CMoveCommand::Create(tDesc);
	if (nullptr == pCommand)
		return E_FAIL;

	HRESULT hr = pQueue->Push(pCommand);
	Safe_Release(pCommand);

	if (FAILED(hr))
		return hr;

	Publish_CommandSelected();
	Enter_Idle();

	return S_OK;
}

HRESULT CBattle_InputDirector::Submit_Switch(_uint iPartyIndex)
{
	if (nullptr == m_pManager)
		return E_FAIL;

	CPlayer_Status* pPlayerState = m_pManager->Get_PlayerState();
	if (nullptr == pPlayerState)
		return E_FAIL;

	PARTY& tParty = pPlayerState->Get_Party();
	POKEMON_INSTANCE* pTarget = PartyOps::Get(tParty, iPartyIndex);
	if (nullptr == pTarget || 0 == pTarget->iSpeciesID || 0 == pTarget->iCurrentHP)
		return E_FAIL;

	CBattler* pPlayer = m_pManager->Get_Battler(g_kBattleSide_Player);
	if (nullptr == pPlayer || nullptr == pPlayer->Get_Instance())
		return E_FAIL;

	if (pPlayer->Get_Instance() == pTarget)
		return E_FAIL;

	CCommandQueue* pQueue = m_pManager->Get_Queue();
	if (nullptr == pQueue)
		return E_FAIL;

	CSwitchCommand::DESC tDesc{};
	tDesc.iActorSide = g_kBattleSide_Player;
	tDesc.iActorSlot = 0;
	tDesc.iTargetPartyIndex = iPartyIndex;

	IBattleCommand* pCommand = CSwitchCommand::Create(tDesc);
	if (nullptr == pCommand)
		return E_FAIL;

	HRESULT hr = pQueue->Push(pCommand);
	Safe_Release(pCommand);

	if (FAILED(hr))
		return hr;

	Publish_CommandSelected();
	Enter_Idle();

	return S_OK;
}

HRESULT CBattle_InputDirector::Submit_Run()
{
	if (nullptr == m_pManager)
		return E_FAIL;

	CCommandQueue* pQueue = m_pManager->Get_Queue();
	if (nullptr == pQueue)
		return E_FAIL;

	CRunCommand::DESC tDesc{};
	tDesc.iActorSide = g_kBattleSide_Player;

	IBattleCommand* pCommand = CRunCommand::Create(tDesc);
	if (nullptr == pCommand)
		return E_FAIL;

	HRESULT hr = pQueue->Push(pCommand);
	Safe_Release(pCommand);

	if (FAILED(hr))
		return hr;

	Publish_CommandSelected();
	Enter_Idle();

	return S_OK;
}

void CBattle_InputDirector::Publish_CommandSelected()
{
	if (nullptr == m_pManager)
		return;

	CBattle_EventDispatcher* pDispatcher = m_pManager->Get_EventDispatcher();
	if (nullptr == pDispatcher)
		return;

	EVENT_COMMAND_SELECTED tEvent{};
	tEvent.iSide = g_kBattleSide_Player;
	pDispatcher->Publish(tEvent);
}

CBattle_InputDirector* CBattle_InputDirector::Create()
{
	CBattle_InputDirector* pInstance = new CBattle_InputDirector();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBattle_InputDirector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_InputDirector::Free()
{
	m_pManager = nullptr;
	m_pCommandMenu = nullptr;
	m_pMoveMenu = nullptr;
	m_pEntry = nullptr;


	__super::Free();
}