#include "stdafx.h"
#include "State_List_MonoDrive.h"
#include "PhysX_Controller.h"

CState_MonoDrive_Walk::CState_MonoDrive_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_MonoDrive(pActor, pStatemachine)
{
}

HRESULT CState_MonoDrive_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);
	m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	if (m_pBehaviorTree == nullptr)
	{
		Make_BehaviorTree();
	}

	return S_OK;
}

void CState_MonoDrive_Walk::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->getBlackboard()->m_vPosition = m_vPos;
	m_pBehaviorTree->getBlackboard()->m_vTargetPosition = m_vTargetPos;
	m_pBehaviorTree->getBlackboard()->m_fDistanceToTarget = m_fDistance;

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_MonoDrive_Walk::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_MonoDrive_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_MonoDrive_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_MonoDrive_Walk::isValid_NextState(CState* state)
{
	return true;
}

HRESULT CState_MonoDrive_Walk::Make_BehaviorTree()
{
	FUNCTION_NODE Condition_Distance_Attack = FUNCTION_NODE_MAKE
	{
		cout << "MonoDrive_Condition_Attack" << endl;

		if (InRange(pBlackboard->m_fDistanceToTarget, 0.f, 15.f,"[)"))
		{
			_int iRandom = Random({ 1,1,2,2,3,3,3,4,4,4});
			pBlackboard->setInt("RandomKey", iRandom);

			return BT_STATUS::Success;
		}

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Front = FUNCTION_NODE_MAKE
	{
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
		if (pBlackboard->getInt("RandomKey") == 3)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Condition_Right = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 4)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Call_Front = FUNCTION_NODE_MAKE
	{
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_Walk01_0")
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_0", 1.5f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_0", 1.5f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition); 
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 0.2f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Back = FUNCTION_NODE_MAKE
	{
		cout << "MonoDrive_Back" << endl;
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_WalkB01_1")
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkB01_1", 1.5f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkB01_1", 1.5f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * 0.2f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Left = FUNCTION_NODE_MAKE
	{
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_WalkL01_1")
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkL01_1", 1.5f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkL01_1", 1.5f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Left(fTimeDelta * 0.2f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Right = FUNCTION_NODE_MAKE
	{
		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName() != "Main|B_WalkR01_1")
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkR01_1", 1.5f, false);

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);

		if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
			m_pActor_ModelCom.lock()->Set_Animation("Main|B_WalkR01_1", 1.5f, false);

		m_pActor_TransformCom.lock()->Look_At_OnLand(vTargetPosition);
		m_pActor_TransformCom.lock()->Go_Right(fTimeDelta * 0.2f);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Shoot = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 2 || pBlackboard->getInt("RandomKey") == 4)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_MonoDrive_Shoot>();

			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Call_Drill = FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("RandomKey") == 1 || pBlackboard->getInt("RandomKey") == 3)
		{
			m_pStateMachineCom.lock()->Enter_State<CState_MonoDrive_Drill>();
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	m_pBehaviorTree = Builder()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Distance_Attack)
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Front)
							.decorator<Repeater>(60)
								.leaf<FunctionNode>(Call_Front)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Back)
							.decorator<Repeater>(60)
								.leaf<FunctionNode>(Call_Back)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Left)
							.decorator<Repeater>(60)
								.leaf<FunctionNode>(Call_Left)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Right)
							.decorator<Repeater>(60)
								.leaf<FunctionNode>(Call_Right)
							.end()
						.end()
					.end()
					.leaf<FunctionNode>(Condition_Distance_Attack)
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Front)
							.decorator<Repeater>(70)
								.leaf<FunctionNode>(Call_Front)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Back)
							.decorator<Repeater>(70)
								.leaf<FunctionNode>(Call_Back)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Left)
							.decorator<Repeater>(70)
								.leaf<FunctionNode>(Call_Left)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Right)
							.decorator<Repeater>(70)
								.leaf<FunctionNode>(Call_Right)
							.end()
						.end()
					.end()
					.leaf<FunctionNode>(Condition_Distance_Attack)
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Front)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_Front)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Back)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_Back)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Left)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_Left)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Right)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_Right)
							.end()
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Call_Shoot)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Call_Drill)
						.end()
					.end()
				.end()
			.build();

	return S_OK;
}

void CState_MonoDrive_Walk::Free()
{
}
