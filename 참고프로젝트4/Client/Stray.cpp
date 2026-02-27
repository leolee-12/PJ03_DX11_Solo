#include "stdafx.h"
#include "Stray.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"
#include "State_List_Stray.h"
#include "StatusComp.h"

#include "UI_Manager.h"

IMPLEMENT_CREATE(CStray)
IMPLEMENT_CLONE(CStray, CGameObject)

CStray::CStray(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CStray::CStray(const CStray& rhs)
	:CMonster(rhs)
{
}

HRESULT CStray::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CStray::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_State()))
		RETURN_EFAIL;

	return S_OK;
}

void CStray::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{ -40.f, 5.f, 15.f,1.f });
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{13.f, 5.f, 13.f,1.f});

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MONSTER_CONTROLLER, 0.5f, 0.5f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY,m_pTransformCom,{ 0.7f,1.f,0.7f }, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,0.8f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;	

		GET_SINGLE(CUI_Manager)->Make_MonsterUI("Stray", shared_from_this());

		m_pModelCom->Get_AnimationComponent()->Create_Add_Animation();
	}

	// 스탯 설정
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Enemy, "StrayTest");
}

void CStray::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
}

void CStray::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStray::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pStatusCom->IsZeroHP())
	{
		m_pStateMachineCom->Set_State<CState_Stray_Dead>();
	}
}

void CStray::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CStray::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CStray::Render()
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

HRESULT CStray::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("EN1000_00_1stray"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Set_PreRotate(XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

		m_pModelCom->Clone_NotifyFromPrototype(TEXT("Prototype_Component_Notify_Stray"));
		m_pModelCom->Regist_ModelCompToNotify(m_pModelCom);
	}

	Ready_DissovleTex(); // 디졸브 텍스쳐 추가

	return S_OK;
}

HRESULT CStray::Ready_State()
{
	m_pStateMachineCom->Add_State<CState_Stray_Idle>();
	m_pStateMachineCom->Add_State<CState_Stray_Shoot>();
	m_pStateMachineCom->Add_State<CState_Stray_Hit>();
	m_pStateMachineCom->Add_State<CState_Stray_Dead>();


	m_pStateMachineCom->Set_State<CState_Stray_Idle>();
	return S_OK;
}

HRESULT CStray::Bind_ShaderResources()
{	

	if (FAILED(CMonster::Bind_ShaderResources()))
		RETURN_EFAIL;
		
	return S_OK;
}

void CStray::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CStray::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CStray::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CStray::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	//if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_ATTACK && typeid(*pThisCol->Get_Owner().lock()->Get_StateMachineCom().lock()->Get_CurState()) != typeid(CState_Stray_Shoot))
	//	m_pStateMachineCom->Set_State<CState_Stray_Hit>();
}

void CStray::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CStray::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CStray::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
	if (iHitPower >= 10)
	{
		if (!m_pStateMachineCom->Is_State< CState_Stray_Hit>() && !Get_StatusCom().lock()->IsZeroHP())
			m_pStateMachineCom->Set_State<CState_Stray_Hit>();
	}

	if (iAddHitPower == 1)
	{
		auto pAnimComp = m_pModelCom->Get_AnimationComponent();

		pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
	}
}

void CStray::Load_Json(const json& In_Json)
{
	__super::Load_Json(In_Json);
}

void CStray::Free()
{
	__super::Free();
}