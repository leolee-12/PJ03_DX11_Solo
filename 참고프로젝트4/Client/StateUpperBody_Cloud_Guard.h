#pragma once
#include "State_Cloud.h"
#include "Client_Defines.h"

BEGIN(Client)

/// <summary>
/// 가드는 상체만 따로 움직이는 상태로 분리한다.
/// </summary>
class CStateUpperBody_Cloud_Guard : public CState_Cloud
{
	INFO_CLASS(CStateUpperBody_Cloud_Guard, CState_Cloud)

public:
	explicit CStateUpperBody_Cloud_Guard(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CStateUpperBody_Cloud_Guard() = default;

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