#include "stdafx.h"
#include "GuardHound.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"
#include "Guard_Hound_BiteColl.h"
#include "Guard_Hound_WhipColl.h"
#include "Guard_Hound_ClawColl.h"
#include "State_List_Guard_Hound.h"
#include "StatusComp.h"

#include "UI_Manager.h"

IMPLEMENT_CREATE(CGuardHound)
IMPLEMENT_CLONE(CGuardHound, CGameObject)

CGuardHound::CGuardHound(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CGuardHound::CGuardHound(const CGuardHound& rhs)
	:CMonster(rhs)
{
}

HRESULT CGuardHound::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CGuardHound::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_PartObjects()))
		RETURN_EFAIL;

	if(FAILED(Ready_State()))
		RETURN_EFAIL;

	m_pStatusCom->Init_HP(5.f);

	return S_OK;
}

void CGuardHound::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ -38.f, 5.f, 17.f,1.f }); 
	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{11.f, 5.f, 11.f,1.f});*/

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MONSTER_CONTROLLER, 0.01f, 1.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;	

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY,m_pTransformCom,{1.2f,1.0f,1.7f}, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,0.5f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;	

		GET_SINGLE(CUI_Manager)->Make_MonsterUI("GuardHound", shared_from_this());
				
		//m_pGameInstance->Make_Light_On_Owner(shared_from_this(), _float4(1.f, 0.f, 0.f, 1.f));
		m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	}

	// 스탯 설정
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Enemy, "Hound1");
}

void CGuardHound::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	
}

void CGuardHound::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CGuardHound::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_Guard_Hound_Dead>();
	}
}

void CGuardHound::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CGuardHound::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CGuardHound::Render()
{
	if (m_pModelCom)
	{
		if (FAILED(Bind_ShaderResources()))
			RETURN_EFAIL;

		if (FAILED(CMonster::Render()))
			RETURN_EFAIL;
	}
	return S_OK;
}

HRESULT CGuardHound::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("EN2000_00_GuardHound"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_GuardHound"));
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}

	m_pModelCom->Set_PreRotate(XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

	Ready_DissovleTex(); // 디졸브 텍스쳐 추가

	return S_OK;
}

HRESULT CGuardHound::Ready_PartObjects()
{
	CPartObject::PARTOBJECT_DESC Desc;
	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("C_FaceBase_a");

	if (FAILED(Add_PartObject<CGuard_Hound_BiteColl>(TEXT("Prototype_GameObject_Guard_Hound_Bite"), TEXT("Part_Bite"), &Desc, nullptr)))
		RETURN_EFAIL;

	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("C_CrestA_End");

	if (FAILED(Add_PartObject<CGuard_Hound_WhipColl>(TEXT("Prototype_GameObject_Guard_Hound_Whip"), TEXT("Part_Whip"), &Desc, nullptr)))
		RETURN_EFAIL;

	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("L_FToe_End");

	if (FAILED(Add_PartObject<CGuard_Hound_ClawColl>(TEXT("Prototype_GameObject_Guard_Hound_Claw"), TEXT("Part_Claw"), &Desc, nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CGuardHound::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Idle>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Walk>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Run>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Bite>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Whip>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Claw>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Hit>();
	m_pStateMachineCom->Add_State<CState_Guard_Hound_Dead>();

	m_pStateMachineCom->Set_State<CState_Guard_Hound_Idle>();

	return S_OK;
}

HRESULT CGuardHound::Bind_ShaderResources()
{	
	//// 공통 행렬 바인딩
	//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix",&m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
	//	RETURN_EFAIL;
	//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix",&m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
	//	RETURN_EFAIL;
	//		
	//// 월드 행렬 바인딩
	//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix",&m_pTransformCom->Get_WorldFloat4x4())))
	//	RETURN_EFAIL;

	if (FAILED(CMonster::Bind_ShaderResources()))
		RETURN_EFAIL;
		
	return S_OK;
}

void CGuardHound::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CGuardHound::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CGuardHound::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CGuardHound::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (typeid(*m_pStateMachineCom->Get_CurState()) != typeid(CState_Guard_Hound_Dead))
	{
		if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_ATTACK)
		{
			m_pStateMachineCom->Set_State<CState_Guard_Hound_Hit>();
		}
	}

}

void CGuardHound::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CGuardHound::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
	
}

void CGuardHound::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	if (iHitPower >= 10)
	{
		if (!m_pStateMachineCom->Is_State< CState_Guard_Hound_Hit>() && !Get_StatusCom().lock()->IsZeroHP())
			m_pStateMachineCom->Set_State<CState_Guard_Hound_Hit>();
	}

	if (iAddHitPower == 1)
	{
		auto pAnimComp = m_pModelCom->Get_AnimationComponent();

		pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
	}
}

void CGuardHound::Load_Json(const json& In_Json)
{
	__super::Load_Json(In_Json);
}

void CGuardHound::Free()
{
	__super::Free();
}