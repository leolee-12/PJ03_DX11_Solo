#pragma once
#include "State_Cloud.h"
#include "Client_Defines.h"

BEGIN(Client)

class CState_Cloud_LadderUpFinished : public CState_Cloud
{
	INFO_CLASS(CState_Cloud_LadderUpFinished, CState)

public:
	explicit CState_Cloud_LadderUpFinished(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Cloud_LadderUpFinished() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;


private:
	_bool	m_bIsNormal = false;
public:
	GETSET_EX2(_bool, m_bIsNormal, IsNormal, GET_C_REF, SET)

protected:
	virtual void		Free();
};

END