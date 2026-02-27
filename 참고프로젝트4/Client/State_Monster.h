#pragma once
#include "Client_Defines.h"
#include "State.h"

#include "Effect_Manager.h"

#include "Character.h"

BEGIN(Client)

class CState_Monster : public CState
{
	INFO_CLASS(CState_Monster, CState)

public:
	explicit CState_Monster(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Monster() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;
	
protected:
	_float							m_fDistance = { 0.f };
	_float3							m_vTargetPos = {0.f,0.f,0.f};
	_float3							m_vPos = { 0.f,0.f,0.f};
	_float3							m_vDirToTarget = { 0.f,0.f,0.f };	

	shared_ptr<class CGameObject>	m_pTarget = { nullptr };	

	_int							m_iAttackCount = { 0 };

protected:
	virtual void		Free();
};

END