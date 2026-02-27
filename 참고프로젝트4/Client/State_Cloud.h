#pragma once
#include "State_Player.h"
#include "Client_Defines.h"

BEGIN(Client)

class CState_Cloud : public CState_Player
{
	INFO_CLASS(CState_Cloud, CState)
public:
	enum class BATTLE_MODE {ASSERT,BRAVE,NETURAL};

public:
	explicit CState_Cloud(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Cloud() = default;

public:
	virtual HRESULT		Initialize_State(CState* pPreviousState)	override;
	virtual void		Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void		Tick(_cref_time fTimeDelta)					override;
	virtual void		Late_Tick(_cref_time fTimeDelta)			override;
	virtual void		Transition_State(CState* pNextState)		override;
	virtual bool		isValid_NextState(CState* state)			override;


protected:
	string m_strBattleMode[3] = { "Battle00|B_","Battle01|B_","Neutral|N_"};
	string m_strCurBattleMode = "Battle00|B_";
	BATTLE_MODE m_eCurBattleMode = BATTLE_MODE::ASSERT;

	_bool m_bInputBattleMode = false;
	_bool m_bBattleModeChange = false;

public:
	void Update_BattleModeName();
	void Set_CurrentBattleMode(BATTLE_MODE eBattleMode) { m_eCurBattleMode = eBattleMode; }
	string Get_CurrentBattleModeName() const { return m_strCurBattleMode; }
	void Set_CurrentBattleModeName(string strBattleModeName) { m_strCurBattleMode = strBattleModeName;}

protected:
	virtual void		Free();
};

END