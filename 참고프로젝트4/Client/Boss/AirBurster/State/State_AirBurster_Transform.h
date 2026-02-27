#pragma once
#include "../Public/Boss/AirBurster/State/State_AirBurster.h"

#define ROCKETCONDITIONNUM1 0
#define ROCKETCONDITIONNUM2 100

BEGIN(Client)

class CState_AirBurster_Transform final : public CState_AirBurster
{
	INFO_CLASS(CState_AirBurster_Transform, CState_AirBurster)

public:
	enum AIRBURSTER_PATTERN {
		P1_REARMACHINEGUN = 1,
		P1_FRONTMACHINEGUN = 2,
		P1_ENERGYBALL = 3,
		P1_FINGERBEAM = 4,
		P1_EMFIELD = 5,
		P1_TANKBURSTER = 6,
		P1_BURNUR = 7,
		P1_SHOULDERBEAM = 8,
		P1_ROCKETARM = 9,
		P1_Docking = 10,
		P1_Dead = 11,
		P1_PHASE1ENTER = 97,
		P1_PHASE2ENTER = 98,
		P1_PHASE3ENTER = 99,
		P1_PATTERN_END
	};

public:
	explicit CState_AirBurster_Transform(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_AirBurster_Transform() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_uint					m_iCurPhaseNum = { 0 }; // 현재 페이즈를 나타냄
	queue<_int>				m_quePreNum; // 같은거 연속 방지
	AIRBURSTER_PATTERN		m_eCurPattern = { P1_PATTERN_END }; // 현재 패턴을 나타낸다.

private: // Phase1
	_uint					m_iPhase1Num[5] = { 1,2,3,4,5 };

private: // Phase2
	_uint					m_iPhase2Num[4] = { 3,4,7,8 };
	_uint					m_iPhase2RocketArmNum[3] = { 3,6,8 };
	_uint					m_iPhase2Count = { 0 };
	_bool					m_bRocketPattern = { false };

private:
	_bool					m_bCheckPattern = { true };

private:
	_uint RandNum(_uint iPatternNum);

	
private:
	virtual void		Free();
};

END


//HRESULT CState_Tifa_Move2::Initialize_State(CState* pPreviousState)
//{
//	m_pActor_BodyModelCom->Set_Animation("Stand3", true);
//	__super::Initialize_State(pPreviousState);
//
//	if (m_pBehaviorTree == nullptr)
//	{
//		/*_float3 vBaseDir = Random<_float3>({ { 1.f,0.f,0.f }, { -1.f,0.f,0.f }, { 0.f,0.f,1.f }, { 0.f,0.f,-1.f } });
//		m_pActor_TransformCom->Set_BaseDir(vBaseDir);
//		m_pActor_TransformCom->Set_Look(XMLoadFloat3(&vBaseDir));*/
//
//		FUNCTION_NODE Condition_Distance
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Condition_Distance" << endl;
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//
//			if (XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)) <= 15.0f)
//			{
//				return BT_STATUS::Failure;
//			}
//			else
//			{
//				return BT_STATUS::Success;
//			}
//		};
//
//		FUNCTION_NODE Condition_Hit
//			= FUNCTION_NODE_MAKE
//		{
//			if (static_cast<CBoss_Tifa*>(m_pActor)->Get_Hit())
//				return BT_STATUS::Success;
//			else
//				return BT_STATUS::Failure;
//		};
//
//		FUNCTION_NODE Call_Hit
//			= FUNCTION_NODE_MAKE
//		{
//			m_pActor_BodyModelCom->Set_Animation("Stand3", true);
//			return BT_STATUS::Success;
//		};
//		//FUNCTION_NODE Call_Patrol
//		//	= FUNCTION_NODE_MAKE
//		//{
//		//	//cout << "Call_Patrol" << endl;
//		//	if (!m_pActor_TransformCom->Go_Straight(fTimeDelta * 0.4f))
//		//		Turn_Dir(fTimeDelta);
//		//	else
//		//		m_iDir = NONE;
//
//		//	return BT_STATUS::Success;
//		//};
//
//		FUNCTION_NODE Call_TurnToTarget
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_TurnToTarget" << endl;
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//			_vector vLookToTarget = XMVector3Normalize(vTargetPosition - vPosition);
//
//			if (XMVectorGetX(XMVector3Length(XMVector3Normalize(m_pActor_TransformCom->Get_State(CTransform::STATE_LOOK)) - vLookToTarget)) >= 0.05f)
//			{
//				m_pActor_BodyModelCom->Set_Animation("Run2", true);
//				//m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//				_float3 vBaseDir = {};
//				XMStoreFloat3(&vBaseDir, vLookToTarget);
//				m_pActor_TransformCom->Set_BaseDir(vBaseDir);
//
//				m_pActor_TransformCom->Rotate_On_BaseDir(fTimeDelta * 0.1f, XMConvertToRadians(0.f));
//				return BT_STATUS::Success;
//			}
//			else
//				return BT_STATUS::Failure;
//
//		};
//
//		FUNCTION_NODE Call_TurnToTarget_Fast
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_TurnToTarget" << endl;
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//			_vector vLookToTarget = XMVector3Normalize(vTargetPosition - vPosition);
//
//			if (XMVectorGetX(XMVector3Length(XMVector3Normalize(m_pActor_TransformCom->Get_State(CTransform::STATE_LOOK)) - vLookToTarget)) >= 0.05f)
//			{
//				m_pActor_BodyModelCom->Set_Animation("Run2", true);
//				//m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//				_float3 vBaseDir = {};
//				XMStoreFloat3(&vBaseDir, vLookToTarget);
//				m_pActor_TransformCom->Set_BaseDir(vBaseDir);
//
//				m_pActor_TransformCom->Rotate_On_BaseDir(fTimeDelta * 0.5f, XMConvertToRadians(0.f));
//				return BT_STATUS::Success;
//			}
//			else
//				return BT_STATUS::Failure;
//
//		};
//		//FUNCTION_NODE Condition_Sight
//		//	= FUNCTION_NODE_MAKE
//		//{
//		//	//cout << "Condition_Sight" << endl;
//		//	_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//		//	_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//
//		//	_vector vActorToTargetDir = XMVector3Normalize(vTargetPosition - vPosition);
//		//	_vector vLook = XMVector3Normalize( m_pActor_TransformCom->Get_State(CTransform::STATE_LOOK));
//
//		//	if (XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)) <= 6.0f && XMVectorGetX(XMVector3Dot(vActorToTargetDir, vLook)) >= cos(XMConvertToRadians(90.f)))
//		//		return BT_STATUS::Success;
//		//	else
//		//		return BT_STATUS::Failure;
//		//};
//
//		FUNCTION_NODE Condition_Forward_Distance
//			= FUNCTION_NODE_MAKE
//		{
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//
//			if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)),12.f, 15.f,"[]"))
//			{
//
//				return BT_STATUS::Success;
//			}
//			else
//				return BT_STATUS::Failure;
//		};
//
//		FUNCTION_NODE Call_Forward
//			= FUNCTION_NODE_MAKE
//		{
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			if (!InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.0f, 3.0f, "[]"))
//			{
//					m_pActor_BodyModelCom->Set_Animation("Run2", true);
//					m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//					m_pActor_TransformCom->Go_Straight(fTimeDelta * 0.4f);
//			}
//			else
//				m_pActor_BodyModelCom->Set_Animation("Stand3", true);
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Condition_Backward_Distance
//			= FUNCTION_NODE_MAKE
//		{
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//
//			if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.0f, 3.0f, "[]"))
//			{
//				return BT_STATUS::Success;
//			}
//			else
//				return BT_STATUS::Failure;
//		};
//
//		FUNCTION_NODE Call_Backward
//			= FUNCTION_NODE_MAKE
//		{
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//
//			m_pActor_BodyModelCom->Set_Animation("Run2", true);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//			m_pActor_TransformCom->Go_Straight(-fTimeDelta * 0.4f);
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Call_MoveTo
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_MoveTo" << endl;
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//
//			m_pActor_BodyModelCom->Set_Animation("Run2", true);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//			m_pActor_TransformCom->Go_Straight(fTimeDelta * 0.6f);
//			return BT_STATUS::Success;
//		};
//
//
//		//FUNCTION_NODE Call_Revolve
//		//	= FUNCTION_NODE_MAKE
//		//{
//		//	if (m_iNumTurn++ <= 180)
//		//	{
//		//		//cout << "Call_Revolve" << endl;
//		//		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//		//		_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//
//		//		m_pActor_BodyModelCom->Set_Animation("Run2", true);
//		//		m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//		//		m_pActor_TransformCom->Go_Right(fTimeDelta * 0.08f);
//
//		//		return BT_STATUS::Failure;
//		//	}
//		//	else
//		//	{
//		//		m_iNumTurn = 0;
//		//		return BT_STATUS::Success;
//		//	}
//
//		//};
//
//
//		FUNCTION_NODE Call_Attack_Turn
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_Attack1" << endl;
//			/*_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);*/
//
//			m_pStateMachineCom->Enter_State<CState_Tifa_Attack2_Turn>();
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Call_Attack_Sphere
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_Attack2" << endl;
//			/*_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);*/
//
//			m_pStateMachineCom->Enter_State<CState_Tifa_Attack2_Sphere>();
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Call_Attack_Delayed
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_Attack2" << endl;
//			/*_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);*/
//
//			m_pStateMachineCom->Enter_State<CState_Tifa_Attack2_Delayed>();
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Call_Attack_Cage
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_Attack2" << endl;
//			/*_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);*/
//
//			m_pStateMachineCom->Enter_State<CState_Tifa_Attack2_Cage>();
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Call_Attack_Forward
//			= FUNCTION_NODE_MAKE
//		{
//			//cout << "Call_Attack2" << endl;
//			/*_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);*/
//
//			m_pStateMachineCom->Enter_State<CState_Tifa_Attack2_Forward>();
//			return BT_STATUS::Success;
//		};
//
//		FUNCTION_NODE Call_LookAt_Stand
//			= FUNCTION_NODE_MAKE
//		{
//			_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//			_vector vPosition = XMLoadFloat3(&pBlackboard->m_vPosition);
//
//			if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 10.0f, 12.0f, "[]"))
//			{
//				//cout << "Call_LookAt" << endl;
//				m_pActor_BodyModelCom->Set_Animation("Stand3", true);
//				_vector vTargetPosition = XMLoadFloat3(&pBlackboard->m_vTargetPosition);
//				m_pActor_TransformCom->Look_At_OnLand(vTargetPosition);
//				return BT_STATUS::Success;
//			}
//			else
//				return BT_STATUS::Failure;
//		};
//
//
//		m_pBehaviorTree = Builder()
//			.composite<Selector>()
//			.composite<Sequence>()
//			.leaf<FunctionNode>(Condition_Hit)
//			.decorator<Repeater>(80)
//			.leaf<FunctionNode>(Call_Hit)
//			.end()
//			.end()
//			.composite<Sequence>()
//			.leaf<FunctionNode>(Condition_Distance)
//			.composite<Selector>()
//			.leaf<FunctionNode>(Call_TurnToTarget)
//			.leaf<FunctionNode>(Call_MoveTo)
//			.end()
//			.end()
//			.composite<Selector>()
//			.leaf<FunctionNode>(Call_TurnToTarget)
//			.composite<Sequence>()
//			.leaf<FunctionNode>(Condition_Backward_Distance)
//			.decorator<Repeater>(80)
//			.leaf<FunctionNode>(Call_Backward)
//			.end()
//			.end()
//			.composite<Sequence>()
//			.leaf<FunctionNode>(Condition_Forward_Distance)
//			.decorator<Repeater>(80)
//			.leaf<FunctionNode>(Call_Forward)
//			.end()
//			.end()
//			.leaf<FunctionNode>(Call_LookAt_Stand)
//			.leaf<FunctionNode>(Call_TurnToTarget_Fast)
//			.composite<StatefulSelector>()
//			.leaf<FunctionNode>(Call_Attack_Delayed)
//			.decorator<Repeater>(50)
//			.leaf<FunctionNode>(Call_LookAt_Stand)
//			.end()
//			.leaf<FunctionNode>(Call_Attack_Turn)
//			.decorator<Repeater>(30)
//			.leaf<FunctionNode>(Call_LookAt_Stand)
//			.end()
//			.leaf<FunctionNode>(Call_Attack_Cage)
//			.leaf<FunctionNode>(Call_Attack_Sphere)
//			.decorator<Repeater>(60)
//			.leaf<FunctionNode>(Call_LookAt_Stand)
//			.end()
//			.leaf<FunctionNode>(Call_Attack_Forward)
//			.decorator<Repeater>(40)
//			.leaf<FunctionNode>(Call_LookAt_Stand)
//			.end()
//			.leaf<FunctionNode>(Call_Attack_Turn)
//			.leaf<FunctionNode>(Call_Attack_Forward)
//			.decorator<Repeater>(60)
//			.leaf<FunctionNode>(Call_LookAt_Stand)
//			.end()
//			.leaf<FunctionNode>(Call_Attack_Delayed)
//			.end()
//			.end()
//			.end()
//			.build();
//
//	}
//
//
//	return S_OK;
//}
//
//void CState_Tifa_Move2::Priority_Tick(_float fTimeDelta)
//{
//	__super::Priority_Tick(fTimeDelta);
//
//	_vector vTargetPosition = m_pTarget_TransformCom->Get_State(CTransform::STATE_POSITION);
//	_vector vPosition = m_pActor_TransformCom->Get_State(CTransform::STATE_POSITION);
//
//	if (XMVectorGetZ(vTargetPosition) >= 24.833)
//	{
//		XMStoreFloat3(&m_pBehaviorTree->getBlackboard()->m_vPosition, vPosition);
//		XMStoreFloat3(&m_pBehaviorTree->getBlackboard()->m_vTargetPosition, vTargetPosition);
//		m_pBehaviorTree->update(fTimeDelta);
//	}
//	else
//	{
//		m_pActor_BodyModelCom->Set_Animation("Stand3", true);
//	}
//}