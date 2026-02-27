#pragma once
#include "State_Cloud.h"
#include "Client_Defines.h"

BEGIN(Client)

/// <summary>
/// 가드로 진입하는 상태
/// </summary>
class CState_Cloud_Guard : public CState_Cloud
{
	INFO_CLASS(CState_Cloud_Guard, CState)

public:
	explicit CState_Cloud_Guard(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Cloud_Guard() = default;

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