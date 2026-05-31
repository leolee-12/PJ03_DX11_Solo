#pragma once
#include "Battle_EventListenerBase.h"

NS_BEGIN(Game_PKM)
class CBattle_Manager;
class CBattle_CommandMenu;
class CBattle_MoveMenu;
class CEntry;

/* CBattle_InputDirector
   - 플레이어 입력 흐름 통제 (MAIN ↔ MOVE 모드 전환 + 결정 시 Queue.Push)
   - EventListener 상속 - EVENT_TURN_STARTED 수신 시 MAIN 모드 진입 (메뉴 활성)
   - 명령 결정 시 자체적으로 Queue.Push + EVENT_COMMAND_SELECTED 발행 후 IDLE 모드 (메뉴 비활성)
   - CommandMenu / MoveMenu / Manager 는 weak (UI Hub / Level / Level owns)
   - 본 트랙 정책:
	   MAIN 에서 FIGHT  -> MOVE 진입
	   MAIN 에서 POKE/BAG -> 무시 (메뉴 재포커스)
	   MAIN 에서 CANCEL -> 도망 (CRunCommand)
	   MOVE 에서 슬롯  -> CMoveCommand (PP 0 이면 무시)
	   MOVE 에서 CANCEL -> MAIN 복귀 */
class CBattle_InputDirector final : public CBattle_EventListenerBase
{
public:
	enum class MODE { IDLE, MAIN, MOVE, ENTRY, END };

private:
	CBattle_InputDirector();
	virtual ~CBattle_InputDirector() = default;

public:
	HRESULT Initialize();

	void Bind(CBattle_Manager* pManager,
		CBattle_CommandMenu* pCommandMenu,
		CBattle_MoveMenu* pMoveMenu,
		CEntry* pEntry);

	/* Level_Battle::Update 의 UI_Update_All 뒤에 호출.
   모드 전환 요청을 한 프레임 지연시켜 같은 프레임 키 입력 race 회피. */
	void Tick(_float fTimeDelta);

	MODE Get_Mode() const { return m_eMode; }

	// EventListener 훅
	virtual void On_TurnStarted(const EVENT_TURN_STARTED& tEvent) override;

private:
	// 메뉴 콜백
	void Handle_CommandActivate(_int iIndex);
	void Handle_CommandCancel();
	void Handle_MoveActivate(_int iIndex);
	void Handle_MoveCancel();
	void Handle_EntryActivate(_int iIndex);
	void Handle_EntryCancel();

	// 모드 전환
	void Enter_Main();
	void Enter_Move();
	void Enter_Idle();
	void Enter_Entry();

	// 명령 발행
	HRESULT Submit_Move(_uint iMoveSlot);
	HRESULT Submit_Switch(_uint iPartyIndex);
	HRESULT Submit_Run();

	// COMMAND_SELECTED 발행 헬퍼
	void Publish_CommandSelected();

private:
	CBattle_Manager* m_pManager = { nullptr };			// weak
	CBattle_CommandMenu* m_pCommandMenu = { nullptr };	// weak (UI Hub owns)
	CBattle_MoveMenu* m_pMoveMenu = { nullptr };		// weak (UI Hub owns)
	CEntry* m_pEntry = { nullptr };						// weak (UI Hub owns)

	MODE m_eMode = { MODE::IDLE };
	MODE m_ePendingMode = { MODE::IDLE };
	_bool m_bModeChangePending = { false };

public:
	static CBattle_InputDirector* Create();

private:
	virtual void Free() override;
};

NS_END