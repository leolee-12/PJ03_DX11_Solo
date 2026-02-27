#pragma once
#include "State_Cloud.h"
#include "Utility/LogicDeviceBasic.h"

class CState_Cloud_AI_AttackMode : public CState_Cloud
{
	INFO_CLASS(CState_Cloud_AI_AttackMode, CState)

public:
	explicit CState_Cloud_AI_AttackMode(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Cloud_AI_AttackMode() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_uint					m_iDamageCount = { 0 };
	_bool					m_bIsExcute = { false };
	FTimeChecker			m_fTimeMax = { 1.f };
private:
	void					Build_BehaviorTree();

protected:
	virtual void		Free();
};

