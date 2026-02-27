#pragma once
#include "Client_Defines.h"
#include "State_Soldier_Gun.h"

BEGIN(Client)

class CState_Soldier_Gun_Hit : public CState_Soldier_Gun
{
	INFO_CLASS(CState_Soldier_Gun_Hit, CState_Soldier_Gun)

public:
	explicit CState_Soldier_Gun_Hit(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Soldier_Gun_Hit() = default;

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