#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_EMField.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_EMField.h"
#include "CommonModelComp.h"

#include "GameInstance.h"

CState_AirBurster_EMField::CState_AirBurster_EMField(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_EMField::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkHover01", 1.f * AIRBURSTERANIMSPEED, false);

	SetUp_AI_Action_Num(AI_ACTION_RUNAWAY);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Burner_EMField
			= FUNCTION_NODE_MAKE
		{
			// L
			{
				/*shared_ptr<CAirBurster_EMField> pBullet = { nullptr };
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
					TEXT("Prototype_GameObject_AirBurster_EMField"), nullptr, &pBullet);
				pBullet->Set_Owner(m_pActor);
				_matrix MuzzleMatrix = m_pActor.lock()->Get_ModelCom().lock()->
					Get_BoneTransformMatrixWithParents(TEXT("L_VFXMuzzleC_a"));
				pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
				pBullet->Get_TransformCom().lock()->Set_Look(MuzzleMatrix.r[2]);*/
			}

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_RealMachineGun
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_RearMachineGun>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_RealMachineGun)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_EMField::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_EMField::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bCheck && m_pActor_ModelCom.lock()->IsAnimation_UpTo(67.f))
	{
		m_bCheck = false;

		shared_ptr<CAirBurster_EMField> pBullet = { nullptr };
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
			TEXT("Prototype_GameObject_AirBurster_EMField"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pActor);
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta,
			m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION));
	}
}

void CState_AirBurster_EMField::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_EMField::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	//m_bCheck = true;
}

bool CState_AirBurster_EMField::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_EMField::Free()
{
}
