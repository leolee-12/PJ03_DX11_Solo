#pragma once

#include "Client_Defines.h"
#include "Aerith/State/State_Aerith.h"


BEGIN(Client)

class CState_Aerith_Walk : public CState_Aerith
{
	INFO_CLASS(CState_Aerith_Walk, CState_Aerith)

public:
	explicit CState_Aerith_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith_Walk() = default;

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