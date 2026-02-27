#pragma once

#include "Client_Defines.h"
#include "Aerith/State/State_Aerith.h"

BEGIN(Client)

/// <summary>
/// 추후 결정 예정.
/// 바레트 팔 나와서 집중포화를 갈겨버리는 공격
/// </summary>
class CState_Aerith_LimitAttack_GuardianOfGuns : public CState_Aerith
{
	INFO_CLASS(CState_Aerith_LimitAttack_GuardianOfGuns, CState_Aerith)

public:
	explicit CState_Aerith_LimitAttack_GuardianOfGuns(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith_LimitAttack_GuardianOfGuns() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;


protected:
	virtual void		Free();

private:
	void Create_AttackArea(_cref_time fTimeDelta);
};

END