#pragma once

#include "Client_Defines.h"
#include "Aerith/State/State_Aerith.h"


BEGIN(Client)

class CState_Aerith_Damage_Catch : public CState_Aerith
{
	INFO_CLASS(CState_Aerith_Damage_Catch, CState_Aerith)

public:
	explicit CState_Aerith_Damage_Catch(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith_Damage_Catch() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
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