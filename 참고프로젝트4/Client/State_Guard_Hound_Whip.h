#pragma once
#include "Client_Defines.h"
#include "State_Guard_Hound.h"

BEGIN(Client)

class CState_Guard_Hound_Whip : public CState_Guard_Hound
{
	INFO_CLASS(CState_Guard_Hound_Whip, CState_Guard_Hound)

public:
	explicit CState_Guard_Hound_Whip(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Guard_Hound_Whip() = default;

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