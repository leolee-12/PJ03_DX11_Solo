#pragma once
#include "Client_Defines.h"
#include "Client_Manager.h"
#include "State.h"
#include "IStatusInterface.h"

BEGIN(Client)

class CState_Player : public CState, public IStatusInterface
{
	INFO_CLASS(CState_Player, CState)

public:
	explicit CState_Player(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Player() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;
	
protected:
	_bool	   m_bPlayable = true;
	DIRKEYINFO m_DirKeyInfo;
	TARGETINFO m_TargetInfo;
	weak_ptr<class CAttackCollider> m_pAttackCollider;

	_bool m_bNextAttack = false;
public:

	_bool	Get_DirKey_Input_Enable();
	void    Set_DirKey_Input_Enable(_bool bEnable);

	_float	DirToRadian(_int iDir);

	void	Turn_Dir();
	_bool	Turn_Dir(_cref_time fTimeDelta);

	void	TurnOn_Weapon();
	void	TurnOff_Weapon();
	void	TurnOnOff_Weapon(_float fFrameMin, _float fFrameMax,_bool bEffect = false);
	
	void	TurnOn_WeaponEffect();
	void	TurnOff_WeaponEffect();

	void	TurnOn_MotionBlur(_float fFrame = -1.f, _bool bUseActorCenter = true ,_float vBlurScale = 0.125f, _float2 vCenter = { 0.5f,0.5f });
	void	TurnOff_MotionBlur(_float fFrame = -1.f);

	void    Check_UseCommand();

	GETSET_EX2(DIRKEYINFO, m_DirKeyInfo, DirKeyInfo, GET_C_REF, SET)
	GETSET_EX2(TARGETINFO, m_TargetInfo, TargetInfo, GET_C_REF, SET)

#pragma region IStatusInterface
private:
	// ³Í ¾²Áö¸»¾î
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) override { return S_OK; }
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName) override { return S_OK; }
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) override {}
public:
	virtual weak_ptr<class CStatusComp> Get_StatusCom() override { return m_pStatusCom; }

protected:
	weak_ptr<class CStatusComp>	m_pStatusCom;
#pragma endregion


protected:
	virtual void		Free();
};

END