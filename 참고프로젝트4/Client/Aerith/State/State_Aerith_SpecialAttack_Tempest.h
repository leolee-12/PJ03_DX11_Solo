#pragma once

#include "Client_Defines.h"
#include "Aerith/State/State_Aerith.h"


BEGIN(Client)

/// <summary>
/// SpecialAttack과 같지만 생성하는 투사체가 템페스트 공격임
/// </summary>
class CState_Aerith_SpecialAttack_Tempest : public CState_Aerith
{
	INFO_CLASS(CState_Aerith_SpecialAttack_Tempest, CState_Aerith)

public:
	explicit CState_Aerith_SpecialAttack_Tempest(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith_SpecialAttack_Tempest() = default;

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
	void Create_Tempest(_cref_time fTimeDelta);
};

END