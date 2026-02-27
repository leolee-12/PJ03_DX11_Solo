#pragma once
#include "Client_Defines.h"
#include "State_Sweeper.h"
#include "Utility/LogicDeviceBasic.h"

BEGIN(Client)

class CState_Sweeper_Flame : public CState_Sweeper
{
	INFO_CLASS(CState_Sweeper_Flame, CState_Sweeper)

public:
	explicit CState_Sweeper_Flame(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Sweeper_Flame() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	FTimeChecker	m_fTimeChecker = FTimeChecker(0.05f);
	FTimeChecker	m_fTimeChecker2 = FTimeChecker(0.15f);

	_bool			m_bFlame = true;

protected:
	virtual void		Free();
};

END