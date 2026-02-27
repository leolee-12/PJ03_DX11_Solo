#pragma once

#include "Client_Defines.h"
#include "State_Player.h"


BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CState_Aerith : public CState_Player
{
	INFO_CLASS(CState_Aerith, CState_Player)
public:
	enum class BATTLE_MODE {ASSERT,BRAVE};

public:
	explicit CState_Aerith(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;


protected:
	string m_strBattleMode[2] = { "Battle00|","Battle01|" };
	string m_strCurBattleMode = "Battle00|";
	BATTLE_MODE m_eCurBattleMode = BATTLE_MODE::ASSERT;

	_bool m_bInputBattleMode = false;
	_bool m_bBattleModeChange = false;

protected:
	_matrix Get_PartObjectBoneMatrix(const wstring& strBoneName);

protected:
	weak_ptr<CPartObject>	m_pPartObject;

public:
	void Update_BattleModeName();

protected:
	virtual void		Free();
};

END