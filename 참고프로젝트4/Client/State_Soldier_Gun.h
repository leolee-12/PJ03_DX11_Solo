#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)

class CState_Soldier_Gun : public CState_Monster
{
	INFO_CLASS(CState_Soldier_Gun, CState_Monster)

public:
	explicit CState_Soldier_Gun(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Soldier_Gun() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

protected:
	virtual void		Free();
};

END