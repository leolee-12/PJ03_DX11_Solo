#include "stdafx.h"
#include "MonoDrive.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"

#include "MonoDrive_DrillColl.h"

#include "StatusComp.h"
#include "State_List_MonoDrive.h"

#include "UI_Manager.h"

IMPLEMENT_CREATE(CMonoDrive)
IMPLEMENT_CLONE(CMonoDrive, CGameObject)

CMonoDrive::CMonoDrive(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CMonoDrive::CMonoDrive(const CMonoDrive& rhs)
	:CMonster(rhs)
{
}

HRESULT CMonoDrive::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CMonoDrive::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_State()))
		RETURN_EFAIL;

	if (FAILED(Ready_PartObjects()))
		RETURN_EFAIL;
	return S_OK;
}

void CMonoDrive::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 3.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MONSTER_CONTROLLER, 0.01f, 0.3f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY, m_pTransformCom, { 0.5f,0.8f,0.5f }, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,0.5f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;

		GET_SINGLE(CUI_Manager)->Make_MonsterUI("MonoDrive", shared_from_this());

		m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	}

	m_pPhysXControllerCom->Enable_Gravity(true);
	m_pPhysXControllerCom->Set_Gravity(0.1f);
	m_pModelCom->Set_Animation(0, 1.f, true);
	m_pModelCom->Set_Animation("Main|B_Idle01_1", 1.f, true);

	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 스탯 설정
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Enemy, "MonoDriveTest");
}

void CMonoDrive::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_vPos.y - m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < -4.f)
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}

}

void CMonoDrive::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMonoDrive::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_MonoDrive_Dead>();
	}
}

void CMonoDrive::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CMonoDrive::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CMonoDrive::Render()
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

HRESULT CMonoDrive::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("MonoEN3000_00_MonoDrive"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_MonoDrive"));
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}

	m_pModelCom->Set_PreRotate(XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

	Ready_DissovleTex(); // 디졸브 텍스쳐 추가

	return S_OK;
}

HRESULT CMonoDrive::Bind_ShaderResources()
{
	//// 공통 행렬 바인딩
	//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
	//	RETURN_EFAIL;
	//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
	//	RETURN_EFAIL;

	//// 월드 행렬 바인딩
	//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
	//	RETURN_EFAIL;

	if (FAILED(CMonster::Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CMonoDrive::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_MonoDrive_Idle>();
	m_pStateMachineCom->Add_State<CState_MonoDrive_Walk>();
	m_pStateMachineCom->Add_State<CState_MonoDrive_Shoot>();
	m_pStateMachineCom->Add_State<CState_MonoDrive_Drill>();
	m_pStateMachineCom->Add_State<CState_MonoDrive_Hit>();
	m_pStateMachineCom->Add_State<CState_MonoDrive_Dead>();

	m_pStateMachineCom->Set_State<CState_MonoDrive_Idle>();
	return S_OK;
}

HRESULT CMonoDrive::Ready_PartObjects()
{
	CPartObject::PARTOBJECT_DESC Desc;
	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("L_TentacleB_End");

	if (FAILED(Add_PartObject<CMonoDrive_DrillColl>(TEXT("Prototype_GameObject_MonoDrive_Drill"), TEXT("Part_Drill"), &Desc, nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

void CMonoDrive::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CMonoDrive::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CMonoDrive::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CMonoDrive::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CMonoDrive::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CMonoDrive::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CMonoDrive::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	if (iHitPower >= 10)
	{
		if (!m_pStateMachineCom->Is_State<CState_MonoDrive_Hit>() && !Get_StatusCom().lock()->IsZeroHP())
			m_pStateMachineCom->Set_State<CState_MonoDrive_Hit>();
	}

	if (iAddHitPower == 1)
	{
		auto pAnimComp = m_pModelCom->Get_AnimationComponent();

		pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
	}
}

void CMonoDrive::Free()
{
	__super::Free();
}