#pragma once
#include "State_Cloud.h"
#include "Client_Defines.h"

BEGIN(Client)

class CState_Cloud_LimitAttack : public CState_Cloud
{
	INFO_CLASS(CState_Cloud_LimitAttack, CState)

public:
	explicit CState_Cloud_LimitAttack(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Cloud_LimitAttack() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	shared_ptr<CTrail_Buffer>		m_pTrail[2] = {};
	_bool							m_bTrailCreate = { false };

private:
	void	Create_Clo();
	void	Activate_Clo(_bool bActivate);

	void	Clo_Anim_Range(_float fMin, _float fMax);

protected:
	virtual void		Free();
};

END