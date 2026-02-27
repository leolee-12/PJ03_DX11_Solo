#pragma once
#include "State_Cloud.h"
#include "Client_Defines.h"

BEGIN(Client)

/// <summary>
/// 아무것도 하지 않아요~
/// </summary>
class CStateUpperBody_Cloud_Idle : public CState_Cloud
{
	INFO_CLASS(CStateUpperBody_Cloud_Idle, CState_Cloud)

public:
	explicit CStateUpperBody_Cloud_Idle(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CStateUpperBody_Cloud_Idle() = default;

public:
	virtual HRESULT		Initialize_State(CState* pPreviousState)	override;
	virtual void		Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void		Tick(_cref_time fTimeDelta)					override;
	virtual void		Late_Tick(_cref_time fTimeDelta)			override;
	virtual void		Transition_State(CState* pNextState)		override;
	virtual bool		isValid_NextState(CState* state)			override;

protected:
	virtual void		Free();
};

END