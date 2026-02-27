#include "stdafx.h"
#include "State_List_Guard_Hound.h"


CState_Guard_Hound_Run::CState_Guard_Hound_Run(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Monster(pActor, pStatemachine)
{
}

HRESULT CState_Guard_Hound_Run::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_0", 1.f, false);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	if (m_pBehaviorTree == nullptr)
	{
		Make_BehaviorTree();
	}

	return S_OK;
}

void CState_Guard_Hound_Run::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->getBlackboard()->m_vPosition = m_vPos;
	m_pBehaviorTree->getBlackboard()->m_vTargetPosition = m_vTargetPos;
	m_pBehaviorTree->getBlackboard()->m_fDistanceToTarget = m_fDistance;
	m_pBehaviorTree->getBlackboard()->m_vDirToTarget = m_vDirToTarget;

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Guard_Hound_Run::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_Guard_Hound_Run::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Guard_Hound_Run::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Guard_Hound_Run::isValid_NextState(CState* state)
{
	return true;
}

HRESULT CState_Guard_Hound_Run::Make_BehaviorTree()
{
	FUNCTION_NODE Condition_Distance = FUNCTION_NODE_MAKE
	{

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
		_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);

		if (5 < XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)) && XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)) <= 10.0f)
			return BT_STATUS::Failure;

		else
			return BT_STATUS::Success;

	};

	FUNCTION_NODE Condition_Distance_Follow = FUNCTION_NODE_MAKE
	{
		_float fDistance = pBlackboard->m_fDistanceToTarget;

		if (InRange(fDistance, 14.f, 20.f,"[]"))
		{
			return BT_STATUS::Success;
		}

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Hit = FUNCTION_NODE_MAKE
	{
		if (dynamic_pointer_cast<shared_ptr<CGuardHound>>(m_pActor.lock())->get()->Get_Hit())
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Rotation_Left = FUNCTION_NODE_MAKE
	{
		_vector vRight = XMVector3Normalize(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_RIGHT));

		_vector vDir = XMLoadFloat3(&pBlackboard->m_vDirToTarget);

		_float dotProduct = XMVectorGetX(XMVector3Dot(vRight, vDir));

		if (dotProduct <= 0)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Rotation_Right = FUNCTION_NODE_MAKE
	{
		_vector vRight = XMVector3Normalize(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_RIGHT));

		_vector vDir = XMLoadFloat3(&pBlackboard->m_vDirToTarget);

		_float dotProduct = XMVectorGetX(XMVector3Dot(vRight, vDir));

		if (dotProduct > 0)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Around = FUNCTION_NODE_MAKE
	{
		_float fDistance = pBlackboard->m_fDistanceToTarget;

		if (InRange(fDistance, 0.f, 14.f, "[)"))
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Attack = FUNCTION_NODE_MAKE
	{
		_float fDistance = pBlackboard->m_fDistanceToTarget;

		if (InRange(fDistance, 0.f, 6.f, "[]"))
		{
			_int iRandom = Random({1,2,3,1,2,3,1,2,3,1,2,3 });
			pBlackboard->setInt("RandomKey", iRandom);
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Attack2 = FUNCTION_NODE_MAKE
	{
		_float fDistance = pBlackboard->m_fDistanceToTarget;

		if (InRange(fDistance, 5.f, 20.f, "[]"))
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Call_LookAt_Target = FUNCTION_NODE_MAKE
	{
		_vector vTargetPos = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPos);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_LookAt_Stand = FUNCTION_NODE_MAKE
	{
		_int iRandom = rand() % 2;	
		
		_vector vTargetPos = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
	
		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPos);

		if (iRandom ==0 && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Idle02_0" && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Manin|B_Idle03_0")
		{
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle02_0", 1.f,false);
		}
		else if (iRandom == 1 && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Idle02_0" && m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Manin|B_Idle03_0")
		{
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle03_0", 1.f, false);
		}
			return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Follow = FUNCTION_NODE_MAKE
	{
		_vector vTargetPos = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_1", 0.7f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPos);
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 1.f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Turn_Left_90 = FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 1.f);

		m_pActor_TransformCom.lock()->Set_BaseDir(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		{
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run03_1", 0.7f, false);

			return BT_STATUS::Success;
		}
		else
		{
			if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Run03_1")
				m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run03_1", 0.7f, false);

			m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, XMConvertToRadians(-70.f));

			return BT_STATUS::Success;
		}

	};

	FUNCTION_NODE Call_Turn_Right_90 = FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 1.f);

		m_pActor_TransformCom.lock()->Set_BaseDir(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		{
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run03_1", 0.7f, false);

			return BT_STATUS::Success;
		}
		else
		{
			if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Run03_1")
				m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run03_1", 0.7f, false);

			m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, XMConvertToRadians(70.f));

			return BT_STATUS::Success;
		}

	};

	FUNCTION_NODE Call_Walk = FUNCTION_NODE_MAKE
	{
		if (4.f < pBlackboard->m_fDistanceToTarget)
		{
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

			if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Walk01_1")
				m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_1", 0.7f, false);

			if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
				m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_1", 0.7f, false);

			m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 0.3f);

			return BT_STATUS::Success;
		} 
		else
		{
			return BT_STATUS::Failure;
		}

	};

	FUNCTION_NODE Call_Dash = FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Run01_1")
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_1", 0.8f, false);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Run01_1", 0.8f, false);

		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 1.2f);

		return BT_STATUS::Success;
	
	};


	FUNCTION_NODE Call_Bite = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 1)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Bite>();

			return BT_STATUS::Success;
		}
		else
		{
			return BT_STATUS::Failure;
		}
	};

	FUNCTION_NODE Call_Attack_Whip = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 2)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Whip>();

			return BT_STATUS::Success;
		}
		else
		{
			return BT_STATUS::Failure;
		}
	};

	FUNCTION_NODE Call_Attack_BodyBlow = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 3)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Guard_Hound_Claw>();

			return BT_STATUS::Success;
		}
		else
	{

	}
		{
			return BT_STATUS::Failure;
		}
	};

	m_pBehaviorTree = Builder()
		.composite<Selector>()
		/*	.composite<Sequence>()
				.leaf<FunctionNode>(Condition_Hit)
			.end()*/
			//.composite<Sequence>()
			//	.leaf<FunctionNode>(Condition_Distance)
			//.end()
			.composite<Sequence>()
				.leaf<FunctionNode>(Condition_Distance_Follow)
				.leaf<FunctionNode>(Call_Follow)
			.end()
			.composite<Selector>()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Around)
					.composite<StatefulSelector>()
						.decorator<Repeater>(8)
							.leaf<FunctionNode>(Call_Turn_Left_90)
						.end()
						.decorator<Repeater>(8)
							.leaf<FunctionNode>(Call_Turn_Right_90)
						.end()
					.end()
					.composite<Sequence>()
						.decorator<Repeater>(35)
							.leaf<FunctionNode>(Call_Dash)
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Rotation_Right)
							.decorator<Repeater>(32)
								.leaf<FunctionNode>(Call_Turn_Right_90)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Rotation_Left)
							.decorator<Repeater>(32)
								.leaf<FunctionNode>(Call_Turn_Left_90)
							.end()
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Attack)
							.composite<StatefulSelector>()
								.leaf<FunctionNode>(Call_Bite)
								.leaf<FunctionNode>(Call_Attack_Whip)
								.leaf<FunctionNode>(Call_Attack_BodyBlow)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Attack2)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_Walk)
							.end()
							.decorator<Repeater>(23)
								.leaf<FunctionNode>(Call_Turn_Left_90)
							.end()
							.composite<Sequence>()
								.decorator<Repeater>(30)
									.leaf<FunctionNode>(Call_Dash)
								.end()
							.end()
							.composite<Sequence>()
								.decorator<Repeater>(60)
									.leaf<FunctionNode>(Call_LookAt_Stand)
								.end()
							.end()
						.end()
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Attack)
								.composite<StatefulSelector>()
									.leaf<FunctionNode>(Call_Bite)
									.leaf<FunctionNode>(Call_Attack_Whip)
									.leaf<FunctionNode>(Call_Attack_BodyBlow)
								.end()
							.end()
							.composite<Selector>()
								.composite<Sequence>()
									.leaf<FunctionNode>(Condition_Rotation_Right)
									.decorator<Repeater>(18)
										.leaf<FunctionNode>(Call_Turn_Left_90)
									.end()
								.end()
								.composite<Sequence>()
									.leaf<FunctionNode>(Condition_Rotation_Left)
									.decorator<Repeater>(18)
										.leaf<FunctionNode>(Call_Turn_Right_90)
									.end()
								.end()
							.end()
						.end()
						.composite<Sequence>()
							.decorator<Repeater>(30)
								.leaf<FunctionNode>(Call_Dash)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Attack)
							.composite<Selector>()
								.leaf<FunctionNode>(Call_Bite)
								.leaf<FunctionNode>(Call_Attack_Whip)
								.leaf<FunctionNode>(Call_Attack_BodyBlow)
							.end()
						.end()
					.end()
				.end()
			.end()
		.end()
		.build();

	return S_OK;
}

void CState_Guard_Hound_Run::Free()
{
}
