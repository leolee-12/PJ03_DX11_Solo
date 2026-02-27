#include "stdafx.h"
#include "Sweeper_Bombing.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Trail_Effect.h"
#include "Effect_Group.h"
#include "Client_Manager.h"

IMPLEMENT_CREATE(CSweeper_Bombing)
IMPLEMENT_CLONE(CSweeper_Bombing, CGameObject)
CSweeper_Bombing::CSweeper_Bombing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CSweeper_Bombing::CSweeper_Bombing(const CSweeper_Bombing& rhs)
	: CBullet(rhs)
{
}

HRESULT CSweeper_Bombing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;


	return S_OK;
}

HRESULT CSweeper_Bombing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSweeper_Bombing::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MAP_DYNAMIC, m_pTransformCom, { 0.4f,0.4f,1.f }, false, nullptr, true);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	m_pModelCom->Transform().Rotation(_float3(1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	// 스킬 설정
	Set_StatusComByOwner("Sweeper_Bombing");
}

void CSweeper_Bombing::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSweeper_Bombing::Tick(_cref_time fTimeDelta)
{
	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);

	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

	if (m_fAccTime > 0.7f)
	{
		m_fAccChaseValue += fTimeDelta * 0.018f * 10.f;

		if (XMVectorGetX(XMVector3Length(pTarget.vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) > 5.f)
		{	
			_float3 vHighOffSet = pTarget.vTargetPos + _float3(3.f, 0.f, 3.f);
			_float3 vLowOffSet = pTarget.vTargetPos - _float3(3.f, 0.f, 3.f);
			m_vPos = m_pGameInstance->RandomFloat3(vLowOffSet, vHighOffSet);
		}
			
		m_pTransformCom->Look_At(m_vPos, m_fAccChaseValue);
	}

	m_pTransformCom->Go_Straight(fTimeDelta * 5.f);
}

void CSweeper_Bombing::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CSweeper_Bombing::Before_Render(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

	__super::Before_Render(fTimeDelta);
}

void CSweeper_Bombing::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSweeper_Bombing::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CSweeper_Bombing::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, L"Sweeper_Missile");
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CSweeper_Bombing::Bind_ShaderResources()
{
	if (m_pModelCom)
	{
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pModelCom->Calculate_TransformFloat4x4FromParent())))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE_ROUGHNESS, "g_MRVTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_EMISSION_COLOR, "g_EmissiveTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_AMBIENT_OCCLUSION, "g_AOTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_OPACITY, "g_AlphaTexture")))
				RETURN_EFAIL;

			if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}

	return S_OK;
}

void CSweeper_Bombing::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bombing::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bombing::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSweeper_Bombing::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_WALL)
	{
		// 대미지 주는 함수, IStatusInterface 인터페이스로 구현됨.
		if (bUseDamage && pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		//weak_ptr<CEffect> pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect_Group>(TEXT("Cloud_Bullet_Hit"), L_EFFECT, nullptr, shared_from_this());
		//pEffect.lock()->Get_TransformCom().lock()->Set_WorldFloat4x4(pEffect.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4() *
		//	m_pTransformCom->Get_WorldFloat4x4());
		Set_Dead();

		shared_ptr<CGameObject> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Sweeper_Bomb"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pOwner.lock());
		pBullet->Get_TransformCom().lock()->Set_Position(1.f, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

void CSweeper_Bombing::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSweeper_Bombing::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CSweeper_Bombing::Free()
{
	__super::Free();
}
