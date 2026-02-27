#include "stdafx.h"
#include "State_List_Sweeper.h"


CState_Sweeper_Walk::CState_Sweeper_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Sweeper(pActor, pStatemachine)
{
}

HRESULT CState_Sweeper_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_2", 1.2f, false);

	if (m_pBehaviorTree == nullptr)
	{
		Make_BehaviorTree();
	}

	return S_OK;
}

void CState_Sweeper_Walk::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->getBlackboard()->m_vPosition = m_vPos;
	m_pBehaviorTree->getBlackboard()->m_vTargetPosition = m_vTargetPos;
	m_pBehaviorTree->getBlackboard()->m_fDistanceToTarget = m_fDistance;
	m_pBehaviorTree->getBlackboard()->m_vDirToTarget = m_vDirToTarget;

	m_pBehaviorTree->update(fTimeDelta);

	m_pActor_TransformCom.lock()->Set_BaseDir(XMVector3Normalize(m_vDirToTarget));
	m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta * 0.5f, 0.f);
}

void CState_Sweeper_Walk::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Sweeper_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Sweeper_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Sweeper_Walk::isValid_NextState(CState* state)
{
	return true;
}

HRESULT CState_Sweeper_Walk::Make_BehaviorTree()
{
	FUNCTION_NODE Condition_End_Reload = FUNCTION_NODE_MAKE
	{
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() == "Main|B_Idle01_2")
		{
			if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(85))
				return BT_STATUS::Success;
			else
				return BT_STATUS::Failure;
		}
		//return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Distance_Attack = FUNCTION_NODE_MAKE
	{
		_int iRandom = Random/*({1,2,3,4,5});*/({1,1,1,1,1,2,2,2,3,3,4,5,5});
		pBlackboard->setInt("RandomKey", iRandom);
		
		return BT_STATUS::Success; 	
	};

	FUNCTION_NODE Condition_Shoot = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 1)
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_MachineGun = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 2)
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Flame = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 3)
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Bombing = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 4)
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Rush = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 5)
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Call_LShoot = FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_LShoot>();
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_RShoot = FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_RShoot>();
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_MachineGun = FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_MachineGun>();
		return BT_STATUS::Success;
	};;

	FUNCTION_NODE Call_Flame = FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Flame>();
		return BT_STATUS::Success;

	};

	FUNCTION_NODE Call_Bombing = FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Bombing>();
		return BT_STATUS::Success;
	
	};

	FUNCTION_NODE Call_Rush = FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Sweeper_Rush>();
		return BT_STATUS::Success;

	};

	m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Condition_End_Reload)
				.leaf<FunctionNode>(Condition_Distance_Attack)
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Shoot)
						.composite<StatefulSelector>()
						.leaf<FunctionNode>(Call_LShoot)
						.leaf<FunctionNode>(Call_RShoot)
						.end()
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_MachineGun)
						.leaf<FunctionNode>(Call_MachineGun)
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Flame)
						.leaf<FunctionNode>(Call_Flame)
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Bombing)
						.leaf<FunctionNode>(Call_Bombing)
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Rush)
						.leaf<FunctionNode>(Call_Rush)
					.end()
				.end()
			.end()
		.build();
	return S_OK;
}

void CState_Sweeper_Walk::Free()
{
}
