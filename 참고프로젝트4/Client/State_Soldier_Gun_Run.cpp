#include "stdafx.h"
#include "State_List_Soldier_Gun.h"


CState_Soldier_Gun_Run::CState_Soldier_Gun_Run(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Soldier_Gun(pActor, pStatemachine)
{
}

HRESULT CState_Soldier_Gun_Run::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	if (m_pBehaviorTree == nullptr)
	{
		Make_BehaviorTree();
	}

	return S_OK;
}

void CState_Soldier_Gun_Run::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->getBlackboard()->m_vPosition = m_vPos;
	m_pBehaviorTree->getBlackboard()->m_vTargetPosition = m_vTargetPos;
	m_pBehaviorTree->getBlackboard()->m_fDistanceToTarget = m_fDistance;

	m_pBehaviorTree->update(fTimeDelta);

	
}

void CState_Soldier_Gun_Run::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_Soldier_Gun_Run::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Soldier_Gun_Run::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);	
}

bool CState_Soldier_Gun_Run::isValid_NextState(CState* state)
{
	return true;
}

HRESULT CState_Soldier_Gun_Run::Make_BehaviorTree()
{

	FUNCTION_NODE Condition_Distance_ForWard = FUNCTION_NODE_MAKE
	{
		//cout << "Condition_Distance_ForWard" << endl;
				
		if (InRange(pBlackboard->m_fDistanceToTarget, 14.f, 25.f,"(]"))
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Front = FUNCTION_NODE_MAKE
	{
		//cout << "Condition_Distance2" << endl;
		if (pBlackboard->getInt("RandomKey") == 1)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Back = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 2)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Left = FUNCTION_NODE_MAKE
	{
		//cout << "Condition_Distance2" << endl;
		if (pBlackboard->getInt("RandomKey") ==3)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Right = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") ==4)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};


	FUNCTION_NODE Condition_Distance_Attack = FUNCTION_NODE_MAKE
	{
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
		if (InRange(pBlackboard->m_fDistanceToTarget, 0.f, 15.f,"[)"))
		{
			_int iRandom = Random({ 1,1,2,3,3,3,4,4,4});
			pBlackboard->setInt("RandomKey", iRandom);

			return BT_STATUS::Success;
		}

		else
			return BT_STATUS::Failure;

	};
	//FUNCTION_NODE Condition_RandomValue = FUNCTION_NODE_MAKE
	//{
	//	_int iRandom = Random({ 1,1,1,2,2,2,3,3,3,3,4,4,4,4});
	//	pBlackboard->setInt("RandomKey", iRandom);

	//	return BT_STATUS::Success;

	//};

	FUNCTION_NODE Condition_Distance_Far = FUNCTION_NODE_MAKE
	{
		//cout << "Condition_Distance_Far" << endl;

		if (InRange(pBlackboard->m_fDistanceToTarget, 4.f, 10.f,"[)"))
		{
			_int iRandom = Random({1,2,3,4});
			pBlackboard->setInt("RandomKey2", iRandom);

			return BT_STATUS::Success;
		}

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Distance_Near = FUNCTION_NODE_MAKE
	{
		//cout << "Condition_Distance_Near" << endl;

		if (InRange(pBlackboard->m_fDistanceToTarget, 0.f, 4.f,"[)"))
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Call_Move_Target = FUNCTION_NODE_MAKE
	{

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|N_Run01_1")
		m_pActor_ModelCom.lock()->Set_Animation("Main|N_Run01_1", 1.f, false);

		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|N_Run01_1", 1.f, false);

		_vector vTargetPos = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPos);
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta);

		return BT_STATUS::Success;

	};

	FUNCTION_NODE Call_Front = FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Walk01_1")
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_1", 1.f, false);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_1", 1.f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 0.3f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_BackMove = FUNCTION_NODE_MAKE
	{
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_WalkB01_1")
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkB01_1", 1.f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkB01_1", 1.f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * 0.2f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Right = FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_WalkR01_1")
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkR01_1", 1.f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkR01_1", 1.f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Right(fTimeDelta * 0.3f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Left = FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_WalkL01_1")
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkL01_1", 1.f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkL01_1", 1.f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Left(fTimeDelta * 0.3f);

		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Combat = FUNCTION_NODE_MAKE
	{
		//cout << "Call_Tonfa" << endl;

		//m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Dead>();
			m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Combat>();

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Kick = FUNCTION_NODE_MAKE
	{
		cout << "Call_Kick" << endl;

		m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Kick>();

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Shoot1 = FUNCTION_NODE_MAKE
	{
		cout << "Call_Shoot1" << endl;

		if (pBlackboard->getInt("RandomKey2") == 1)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Shoot>();

			return BT_STATUS::Success;
		}
		else
		{
			return BT_STATUS::Failure;
		}
	};	

	FUNCTION_NODE Call_MachinGun = FUNCTION_NODE_MAKE
	{
		//cout << "Call_Shoot2" << endl;

		if (pBlackboard->getInt("RandomKey2") == 2)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_MachineGun>();

			return BT_STATUS::Success;
		}
		else
		{
			return BT_STATUS::Failure;
		}
	};

	FUNCTION_NODE Call_Grenade = FUNCTION_NODE_MAKE
	{
		//cout << "Call_Grenade" << endl;

		if (pBlackboard->getInt("RandomKey2") == 3)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Soldier_Gun_Grenade>();

			return BT_STATUS::Success;
		}
		else
		{
			return BT_STATUS::Failure;
		}
	};

	m_pBehaviorTree = Builder()
			.composite<Selector>()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Distance_ForWard)
					.decorator<Repeater>(10)
						.leaf<FunctionNode>(Call_Move_Target)
					.end()
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Distance_Attack)
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Front)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_Front)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Back)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_BackMove)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Left)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_Left)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Right)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_Right)
							.end()
						.end()
					.end()
					.leaf<FunctionNode>(Condition_Distance_Attack)
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Front)
							.decorator<Repeater>(30)
								.leaf<FunctionNode>(Call_Front)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Back)
							.decorator<Repeater>(30)
								.leaf<FunctionNode>(Call_BackMove)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Left)
							.decorator<Repeater>(30)
								.leaf<FunctionNode>(Call_Left)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Right)
							.decorator<Repeater>(30)
								.leaf<FunctionNode>(Call_Right)
							.end()
						.end()
					.end()
					.leaf<FunctionNode>(Condition_Distance_Attack)
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Front)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_Front)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Back)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_BackMove)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Left)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_Left)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Right)
							.decorator<Repeater>(40)
								.leaf<FunctionNode>(Call_Right)
							.end()
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Distance_Near)
							.composite<StatefulSelector>()
								.leaf<FunctionNode>(Call_Combat)
								.leaf<FunctionNode>(Call_Kick)
							.end()
						.end()
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Distance_Far)
								.composite<Selector>()
									.leaf<FunctionNode>(Call_Shoot1)
									.leaf<FunctionNode>(Call_MachinGun)
									.leaf<FunctionNode>(Call_Grenade)
								.end()
							.end()
						.end()
					.end()
				.end()
			.end()
		.build();

	return S_OK;
}

void CState_Soldier_Gun_Run::Free()
{
}
