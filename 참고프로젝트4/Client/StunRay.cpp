#include "stdafx.h"
#include "StunRay.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"

IMPLEMENT_CREATE(CStunRay)
IMPLEMENT_CLONE(CStunRay, CGameObject)

CStunRay::CStunRay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CStunRay::CStunRay(const CStunRay& rhs)
	:CMonster(rhs)
{
}

HRESULT CStunRay::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CStunRay::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;


	//m_pStateMachineCom->Add_State<CState_StunRay_Idle>();

	//m_pStateMachineCom->Set_State<CState_StunRay_Idle>();

	return S_OK;
}

void CStunRay::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4{0.f, 5.f, 0.f,1.f});

	if (m_pModelCom)
	{
		PHYSXCONTROLLER_DESC ControllerDesc = {};
		PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MONSTER_CONTROLLER, 1.5f, 0.5f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
			TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
			return;

		m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY,m_pTransformCom,{1.f,2.0f,1.f}, false, nullptr, true);
		m_vPhysXColliderLocalOffset = { 0.f,1.0f,0.f };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;	
	}

	m_pModelCom->Set_Animation(0, 1.f, true);
}

void CStunRay::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
		
}

void CStunRay::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStunRay::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CStunRay::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

void CStunRay::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CStunRay::Render()
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

HRESULT CStunRay::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("PC_0000_00_StunRay"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;
	}

	m_pModelCom->Set_PreRotate(XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

	Ready_DissovleTex(); // 디졸브 텍스쳐 추가

	return S_OK;
}

HRESULT CStunRay::Bind_ShaderResources()
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

void CStunRay::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CStunRay::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CStunRay::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CStunRay::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CStunRay::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CStunRay::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CStunRay::Free()
{
	__super::Free();
}