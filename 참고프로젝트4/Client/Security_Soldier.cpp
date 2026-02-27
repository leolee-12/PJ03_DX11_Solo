#include "stdafx.h"
#include "Security_Soldier.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"
#include "Monster,MonsterWeapon_List.h"
#include "State_List_Soldier_Gun.h"
#include "StatusComp.h"
#include "Data_Manager.h"
#include "UI_Manager.h"

IMPLEMENT_CREATE(CSecurity_Soldier)
IMPLEMENT_CLONE(CSecurity_Soldier, CGameObject)

CSecurity_Soldier::CSecurity_Soldier(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CSecurity_Soldier::CSecurity_Soldier(const CSecurity_Soldier& rhs)
	:CMonster(rhs)
{
}

HRESULT CSecurity_Soldier::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CSecurity_Soldier::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_PartObjects()))
		RETURN_EFAIL;

	if (FAILED(Ready_State()))
		RETURN_EFAIL;

	return S_OK;
}

void CSecurity_Soldier::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
	
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MONSTER_CONTROLLER, 0.5f, 0.7f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY,m_pTransformCom,{0.8f,1.8f,0.8f}, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,0.8f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;	

		GET_SINGLE(CUI_Manager)->Make_MonsterUI("Soldier", shared_from_this());

		m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	}

	m_pModelCom->Set_Animation("Main|N_Idle02_0", 1.f, true);

	// 스탯 설정
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Enemy, "Soldier1");

}

void CSecurity_Soldier::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSecurity_Soldier::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CSecurity_Soldier::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_Soldier_Gun_Dead>();
	}
}

void CSecurity_Soldier::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CSecurity_Soldier::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSecurity_Soldier::Render()
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

HRESULT CSecurity_Soldier::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("EN0000_00_SecuritySoldier"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;
	}

	m_pModelCom->Set_PreRotate(XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

	// 노티파이 프로토타입 클로닝 + 모델 컴포넌트 전달(Weak)
	m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_Soldier"));
	m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);

	Ready_DissovleTex(); // 디졸브 텍스쳐 추가

	return S_OK;
}

HRESULT CSecurity_Soldier::Bind_ShaderResources()
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

HRESULT CSecurity_Soldier::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Idle>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Walk>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Run>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Shoot>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_MachineGun>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Grenade>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Kick>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Combat>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Hit>();
	m_pStateMachineCom->Add_State<CState_Soldier_Gun_Dead>();

	m_pStateMachineCom->Set_State<CState_Soldier_Gun_Idle>();

	return S_OK;
}

HRESULT CSecurity_Soldier::Ready_PartObjects()
{
	CPartObject::PARTOBJECT_DESC Desc;
	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("R_Weapon_a");

	if (FAILED(Add_PartObject<CSoldier_Gun>(TEXT("Prototype_GameObject_Soldier_Gun"), TEXT("Part_Gun"), &Desc, nullptr)))
		RETURN_EFAIL;

	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("L_Foot_a");

	if (FAILED(Add_PartObject<CSoldier_Kick>(TEXT("Prototype_GameObject_Soldier_Kick"), TEXT("Part_Kick"), &Desc, nullptr)))
		RETURN_EFAIL;

	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("R_Weapon_a");

	if (FAILED(Add_PartObject<CSoldier_Combat>(TEXT("Prototype_GameObject_Soldier_Combat"), TEXT("Part_Combat"), &Desc, nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

void CSecurity_Soldier::Load_Json(const json& In_Json)
{
	__super::Load_Json(In_Json);
}

void CSecurity_Soldier::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CSecurity_Soldier::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CSecurity_Soldier::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CSecurity_Soldier::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
	  
	if (m_pStateMachineCom->Get_CurState() && typeid(*(m_pStateMachineCom->Get_CurState())) != typeid(CState_Soldier_Gun_Dead))
	{
		if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_ATTACK)
		{
			m_pStateMachineCom->Set_State<CState_Soldier_Gun_Hit>();
		}
	}

}

void CSecurity_Soldier::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSecurity_Soldier::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);

}

void CSecurity_Soldier::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	if (iHitPower >= 10)
	{
		if (!m_pStateMachineCom->Is_State< CState_Soldier_Gun_Hit>() && !Get_StatusCom().lock()->IsZeroHP())
			m_pStateMachineCom->Set_State<CState_Soldier_Gun_Hit>();
	}

	if (iAddHitPower == 1)
	{
		auto pAnimComp = m_pModelCom->Get_AnimationComponent();

		pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Spine_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
	}
}

void CSecurity_Soldier::Free()
{
	__super::Free();
}