#pragma once
#include "Client_Defines.h"
#include "State_Stray.h"

BEGIN(Client)

class CState_Stray_Idle : public CState_Stray
{
	INFO_CLASS(CState_Stray_Idle, CState_Stray)

public:
	explicit CState_Stray_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Stray_Idle() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

protected:
	virtual void		Free();
};

END