#pragma once
#include "State_Cloud.h"
#include "Client_Defines.h"

BEGIN(Client)

class CState_Cloud_Dance : public CState_Cloud
{
	INFO_CLASS(CState_Cloud_Dance, CState)

public:
	explicit CState_Cloud_Dance(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Cloud_Dance() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)			override;
	virtual void			Tick(_cref_time fTimeDelta)					    override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			    override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool		m_bDanceFinished = { false };
	string		m_strDanceName[2] = {"EV_SLU5B_3562|EV_SLU5B_3562_PC0000_00_C0","EV_SLU5B_3562|EV_SLU5B_3562_PC0000_00_C"};
	_uint		m_iDigitType = { 0 };
	_uint 		m_iDanceNameNum = { 900 };


	_float		m_fMakeButtonTime = { 0.5f };
	_float		m_fMakeButtonTimeAcc = { 0.f };

protected:
	virtual void		Free();
};

END