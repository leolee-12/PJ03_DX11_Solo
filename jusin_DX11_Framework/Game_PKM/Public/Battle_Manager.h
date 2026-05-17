#pragma once
#include "Base.h"
#include "Battle_Context.h"

/* -------------------------------------------------- */
// CBattle_Manager : 배틀(세션)의 진행을 책임지는 매니저 클래스
//  1) 한 판의 도메인 상태 보유
//	(BATTLE_ENV / BATTLE_SLOT[] / FIELD_STATE / TURN_CONTEXT / BATTLE_PHASE).
//  2) 외부 데이터 핸들 보유 :  소유 X
//	(CPlayer_Status*, 상대 POKEMON_INSTANCE*, 상대 TRAINER_DATA*).
//  3) 배틀 시각 객체 핸들 보유 : 소유 X
//	(m_pBattlerObj[], m_pTrainerObj[])
//     - Level_Battle이 객체 생성 후 Register_*로 등록
//     - 페이즈 진행 중 턴 결과를 객체에 반영할 때 Get_*로 접근
//  4) 페이즈 진행: Update -> 현재 IBattleState 위임
//  5) 객체 배치 좌표 조회 노출 (Get_TrainerPos/Yaw, Get_PokemonPos/Yaw)
//     - 내부적으로 BattleLayout에 위임. 룰 정보는 매니저 내부에 캡슐화
/* -------------------------------------------------- */

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Game_PKM)
class CPlayer_Status;
class CBattler;
class IBattleState;
class CCommandQueue;
class CDamage_Calculator;
class CBattle_EventDispatcher;
class IBattleAI;
class CBattle_ActionSequencer;
class CBattleMsg;

class CBattle_Manager : public CBase
{
private:
	CBattle_Manager();
	virtual ~CBattle_Manager() = default;

public:
	HRESULT Initialize(const BATTLE_ENV& tEnv);

	HRESULT Bind_PlayerParty(CPlayer_Status* pPlayerState, _uint iLeadSlot);
	HRESULT Bind_OpponentSingle(POKEMON_INSTANCE* pOpponent);
	HRESULT Bind_OpponentTrainer(TRAINER_DATA* pTrainerData);

	void	Begin();
	void	Update(_float fTimeDelta);
	void	Request_State(BATTLE_PHASE ePhase);
	void	Request_Exit();
	_bool	Is_Done() const { return BATTLE_PHASE::DONE == m_ePhase; }
	_bool	Has_Pending_Transition() const { return nullptr != m_pNextState; }

	void    Add_Pacing_Lock();
	void    Release_Pacing_Lock();
	_bool   Is_Pacing_Busy() const { return m_iPacingLocks > 0; }

	const BATTLE_ENV&		Get_Env() const { return m_tEnv; }
	const BATTLE_SLOT&		Get_Slot(_uint iSide) const;
	const FIELD_STATE&		Get_Field() const { return m_tField; }
	const TURN_CONTEXT&		Get_Turn() const { return m_tTurn; }
	BATTLE_PHASE			Get_Phase() const { return m_ePhase; }
	CDamage_Calculator*		Get_Damage_Calculator() const { return m_pDamageCalculator; }
	CBattle_EventDispatcher*	Get_EventDispatcher() const { return m_pEventDispatcher; }
	IBattleAI* Get_AI(_uint iSide) const { return (iSide < g_kBattleSideCount) ? m_pAI[iSide] : nullptr; }

	CBattle_ActionSequencer* Get_Sequencer() const { return m_pSequencer; }
	void Set_BattleMsg(CBattleMsg* pMsg) { m_pBattleMsg = pMsg; }   // weak
	CBattleMsg* Get_BattleMsg() const { return m_pBattleMsg; }

	CBattler* Get_Battler(_uint iSide) const;
	CCommandQueue* Get_Queue() const { return m_pQueue; }

	void Register_BattlerObj(_uint iSide, CGameObject* pObj);
	void Register_TrainerObj(_uint iSide, CGameObject* pObj);

	CGameObject* Get_BattlerObj(_uint iSide) const;
	CGameObject* Get_TrainerObj(_uint iSide) const;

	_float3 Get_TrainerPos(_uint iSide, _uint iSlotIndex = 0) const;
	_float  Get_TrainerYaw(_uint iSide, _uint iSlotIndex = 0) const;
	_float3 Get_PokemonPos(_uint iSide, _uint iSlotIndex = 0) const;
	_float  Get_PokemonYaw(_uint iSide, _uint iSlotIndex = 0) const;

private:
	BATTLE_ENV		m_tEnv = {};
	FIELD_STATE		m_tField = {};
	TURN_CONTEXT	m_tTurn = {};
	BATTLE_PHASE	m_ePhase = { BATTLE_PHASE::INTRO };

	CBattler* m_pBattlers[g_kBattleSideCount] = {};
	IBattleState* m_pCurrentState = { nullptr };
	IBattleState* m_pNextState = { nullptr };
	CCommandQueue* m_pQueue = { nullptr };
	CDamage_Calculator* m_pDamageCalculator = { nullptr };
	CBattle_EventDispatcher* m_pEventDispatcher = { nullptr };
	IBattleAI* m_pAI[g_kBattleSideCount] = {};
	CBattle_ActionSequencer* m_pSequencer = { nullptr };
	CBattleMsg* m_pBattleMsg = { nullptr };  // weak — Level/UI Hub owns

	_int m_iPacingLocks = { 0 };

	CPlayer_Status*		m_pPlayerState = { nullptr };
	POKEMON_INSTANCE*	m_pOpponentSingle = { nullptr };
	TRAINER_DATA*		m_pOpponentTrainer = { nullptr };

	CGameObject* m_pBattlerObj[g_kBattleSideCount] = {};
	CGameObject* m_pTrainerObj[g_kBattleSideCount] = {};

private:
	BATTLE_CONTEXT Build_Context();
	HRESULT Initialize_CoreComponents();
	IBattleState* Create_State(BATTLE_PHASE ePhase);
	void Apply_Pending_Transition(const BATTLE_CONTEXT& ctx);
	void Release_State(IBattleState*& pState);

public:
	static CBattle_Manager* Create(const BATTLE_ENV& tEnv);

protected:
	virtual void Free() override;
};

NS_END