#include "stdafx.h"
#include "Sweeper.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"
#include "Sweeper_RushColl.h"

#include "State_List_Sweeper.h"
#include "StatusComp.h"
#include "Bullet.h"
#include "Common_Shield.h"

#include "UI_Manager.h"

IMPLEMENT_CREATE(CSweeper)
IMPLEMENT_CLONE(CSweeper, CGameObject)

CSweeper::CSweeper(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CSweeper::CSweeper(const CSweeper& rhs)
	:CMonster(rhs)
{
}

HRESULT CSweeper::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CSweeper::Initialize(void* pArg)
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

void CSweeper::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, 
			m_pTransformCom, CL_MONSTER_CONTROLLER, 3.6f, 1.9f,1.9f,PxControllerShapeType::eBOX);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY, 
			m_pTransformCom, { 3.2f, 3.5f, 3.8f }, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f, 0.4f, 0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;	

		GET_SINGLE(CUI_Manager)->Make_MonsterUI("Sweeper", shared_from_this());

	}

	m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	m_pModelCom->Set_Animation(0, 1.f, true);

	m_pModelCom->Set_Animation("Main|B_Idle01_1", 1.f, true);

	// 스탯 설정
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Enemy, "SweeperTest");
}

void CSweeper::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
		
}

void CSweeper::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CSweeper::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_Sweeper_Dead>();
	}
}

void CSweeper::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CSweeper::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSweeper::Render()
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

HRESULT CSweeper::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("EN1001_00_Sweeper"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Set_PreRotate(XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

		m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_Sweeper"));
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}


	Ready_DissovleTex(); // 디졸브 텍스쳐 추가

	return S_OK;
}

HRESULT CSweeper::Bind_ShaderResources()
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

HRESULT CSweeper::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_Sweeper_Idle>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Walk>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Run>();
	m_pStateMachineCom->Add_State<CState_Sweeper_LShoot>();
	m_pStateMachineCom->Add_State<CState_Sweeper_RShoot>();
	m_pStateMachineCom->Add_State<CState_Sweeper_MachineGun>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Flame>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Bombing>();
	m_pStateMachineCom->Add_State<CState_Sweeper_JumpAttack>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Hit>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Dead>();
	m_pStateMachineCom->Add_State<CState_Sweeper_Rush>();

	m_pStateMachineCom->Set_State<CState_Sweeper_Idle>();

	return S_OK;
}

HRESULT CSweeper::Ready_PartObjects()
{
	CPartObject::PARTOBJECT_DESC Desc;
	Desc.pParentTransform = m_pTransformCom;
	Desc.pBoneGroup = m_pModelCom->Get_BoneGroup();
	Desc.strBoneName = TEXT("C_VFXMuzzleA_a");

	if (FAILED(Add_PartObject<CSweeper_RushColl>(TEXT("Prototype_GameObject_Sweeper_RushColl"), 
		TEXT("Part_Rush"), &Desc, nullptr)))
		RETURN_EFAIL;

	return S_OK;
}

void CSweeper::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CSweeper::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CSweeper::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CSweeper::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	if (iStatusDamaged & ECast(EStatusModified::Damage))
	{
		if (iHitPower >= 10)
		{
			if (!m_pStateMachineCom->Is_State< CState_Sweeper_Hit>() && !Get_StatusCom().lock()->IsZeroHP())
				m_pStateMachineCom->Set_State<CState_Sweeper_Hit>();
		}

		if (iAddHitPower == 1)
		{
			auto pAnimComp = m_pModelCom->Get_AnimationComponent();

			pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
		}
	}
	else if (iStatusDamaged & ECast(EStatusModified::Block))
	{
		shared_ptr<CCommon_Shield> pBullet = { nullptr };
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
			TEXT("Prototype_GameObject_Common_Shield"), nullptr, &pBullet);
		pBullet->Set_Owner(shared_from_this());
		pBullet->Get_TransformCom().lock()->Set_Scaling(3.f, 3.f, 3.f);
		DynPtrCast<CCommon_Shield>(pBullet)->Set_Offset({0.f, 1.f, 0.f});

		/*auto pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(TEXT("Common_Shield"), shared_from_this());
		pEffect->Get_TransformCom().lock()->Set_Scaling(3.f, 3.f, 3.f);*/
	}
}

void CSweeper::Free()
{
	__super::Free();
}