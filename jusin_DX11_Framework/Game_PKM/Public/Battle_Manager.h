#pragma once
#include "Base.h"
#include "Game_BattleSession.h"

NS_BEGIN(Game_PKM)
class CPlayerState;

class CBattle_Manager : public CBase
{
private:
	CBattle_Manager();
	virtual ~CBattle_Manager() = default;

public:
	HRESULT Initialize(const BATTLE_ENV& tEnv);

	HRESULT Bind_PlayerParty(CPlayerState* pPlayerState, _uint iLeadSlot);
	HRESULT Bind_OpponentSingle(POKEMON_INSTANCE* pOpponent);
	HRESULT Bind_OpponentTrainer(TRAINER_DATA* pTrainerData);

	void  Update(_float fTimeDelta);
	void  Request_Exit();
	_bool Is_Done() const { return BATTLE_PHASE::DONE == m_ePhase; }

	const BATTLE_ENV&	Get_Env() const { return m_tEnv; }
	const BATTLE_SLOT&	Get_Slot(_uint iSide) const;
	const FIELD_STATE&	Get_Field() const { return m_tField; }
	const TURN_CONTEXT&	Get_Turn() const { return m_tTurn; }
	BATTLE_PHASE		Get_Phase() const { return m_ePhase; }

private:
	void Phase_Intro(_float fTimeDelta);
	void Phase_Input_Player(_float fTimeDelta);
	void Phase_Input_Opponent(_float fTimeDelta);
	void Phase_Resolve_Order(_float fTimeDelta);
	void Phase_Resolve_Action(_float fTimeDelta, _uint iOrderIndex);
	void Phase_Resolve_End(_float fTimeDelta);
	void Phase_Check_End(_float fTimeDelta);
	void Phase_Outro(_float fTimeDelta);

private:
	BATTLE_ENV		m_tEnv = {};
	BATTLE_SLOT		m_tSlot[g_kBattleSideCount] = {};
	FIELD_STATE		m_tField = {};
	TURN_CONTEXT	m_tTurn = {};
	BATTLE_PHASE	m_ePhase = { BATTLE_PHASE::INTRO };

	CPlayerState*		m_pPlayerState = { nullptr };
	POKEMON_INSTANCE*	m_pOpponentSingle = { nullptr };
	TRAINER_DATA*		m_pOpponentTrainer = { nullptr };

public:
	static CBattle_Manager* Create(const BATTLE_ENV& tEnv);

protected:
	virtual void Free() override;
};

NS_END