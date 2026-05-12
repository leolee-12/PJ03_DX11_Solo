#pragma once
#include "Battle_EventListenerBase.h"

NS_BEGIN(Game_PKM)

class CBattle_Manager;
class CBattleMsg;

/* CBattle_MsgListener */
// - Dispatcher 구독자. BATTLE_EVENT_* 를 받아 CBattleMsg 에 표시할 한글 메시지로 변환한다.
// - 연속 발행되는 이벤트는 큐에 적재하고 하나씩 표시한다.
// - 표시 중에는 CBattle_Manager 에 페이싱 락을 잡아 State 전이를 보류시킨다.
// - CBattle_Manager / CBattleMsg 는 weak 참조 (소유자: CLevel_Battle).
class CBattle_MsgListener final : public CBattle_EventListenerBase
{
private:
	CBattle_MsgListener();
	virtual ~CBattle_MsgListener() = default;

public:
	HRESULT Initialize();

	void Bind(CBattle_Manager* pManager, CBattleMsg* pMsg);
	void Tick(_float fTimeDelta);

	virtual void On_BattleStarted(const EVENT_BATTLE_STARTED& tEvent) override;
	virtual void On_MoveUsed(const EVENT_MOVE_USED& tEvent) override;
	virtual void On_MoveFailed(const EVENT_MOVE_FAILED& tEvent) override;
	virtual void On_DamageDealt(const EVENT_DAMAGE_DEALT& tEvent) override;
	virtual void On_PokemonFainted(const EVENT_POKEMON_FAINTED& tEvent) override;
	virtual void On_RunFailed(const EVENT_RUN_FAILED& tEvent) override;
	virtual void On_RunSucceeded(const EVENT_RUN_SUCCEEDED& tEvent) override;
	virtual void On_BattleEnded(const EVENT_BATTLE_ENDED& tEvent) override;

private:
	_wstring Get_BattlerName(_uint iSide) const;
	_wstring Get_MoveName(_uint iMoveID) const;

private:
	CBattle_Manager* m_pManager = { nullptr };       // weak
	CBattleMsg* m_pMsg = { nullptr };           // weak

	std::queue<_wstring>    m_qMessages;
	_bool	m_bLockHeld = { false };

	_float	m_fInterMessageDelay = { 1.2f };        // 메시지 표시
	_float	m_fWaitTimer = { 0.f };
	_bool	m_bWaiting = { false };
	_bool	m_bMessageWaitConsumed = { false };     // 현재 메시지

public:
	static CBattle_MsgListener* Create();

private:
	virtual void Free() override;
};

NS_END