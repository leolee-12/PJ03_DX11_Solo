#pragma once


#include "Client_Defines.h"
#include "Aerith/State/State_Aerith.h"


BEGIN(Client)

class CState_Aerith_SpecialAttack01 : public CState_Aerith
{
	INFO_CLASS(CState_Aerith_SpecialAttack01, CState_Aerith)

public:
	explicit CState_Aerith_SpecialAttack01(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith_SpecialAttack01() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_uint m_iAttackType = 0;
	_bool m_bMousePress = true;
	_bool m_bMouseDown = false;
	_bool m_bMouseRelease = false;

protected:
	virtual void		Free();
};

END