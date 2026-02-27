#include "stdafx.h"
#include "Soldier_Grenade.h"
#include "Client_Manager.h"
#include "Data_Manager.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CSoldier_Grenade)
IMPLEMENT_CLONE(CSoldier_Grenade, CGameObject)

CSoldier_Grenade::CSoldier_Grenade(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CBullet(pDevice, pContext)
{
}

CSoldier_Grenade::CSoldier_Grenade(const CSoldier_Grenade& rhs)
	: CBullet(rhs)
{
}

HRESULT CSoldier_Grenade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSoldier_Grenade::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CSoldier_Grenade::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCONTROLLER_DESC ControllerDesc = {};
	PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MAP_DYNAMIC, 0.001f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
		TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
		return;

	m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner.lock());

	_float3 vLook = pTarget.vTargetPos;
	vLook.y += 4.f;
	m_pTransformCom->Look_At(vLook);

	m_pPhysXControllerCom->Set_Gravity(m_fGravity);

	m_pPhysXControllerCom->Enable_Gravity(false);

}

void CSoldier_Grenade::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}



void CSoldier_Grenade::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pPhysXControllerCom->Set_Gravity(m_fGravity);

	if (!m_bMove)
	{
		if (m_pOwner.lock() != nullptr)
		{
			_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->Get_BoneTransformMatrixWithParents(m_strBoneName);

			m_pTransformCom->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		}
		else
		{

			m_pPhysXControllerCom->Enable_Gravity(true);

			m_fAccTime += fTimeDelta;
		}
	}
	else
	{
		m_pPhysXControllerCom->Enable_Gravity(true);

		m_fAccTime += fTimeDelta;

		if (!m_pPhysXControllerCom->Is_Ground())
		{
			m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
			m_pModelCom->Transform().Turn(_float3(1.f, 0.f, 0.f), fTimeDelta);
		}
		else
		{
			m_fGravity = 8.f;

			m_fSpeed -= fTimeDelta * 7.f;

			if (0 >= m_fSpeed)
				m_fSpeed = 0.f;

			m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
			m_pModelCom->Transform().Turn(_float3(1.f, 0.f, 0.f), fTimeDelta);
		}

	}
}

void CSoldier_Grenade::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fAccTime >= 2.5f)
	{
		Set_Dead();

		shared_ptr<CGameObject> pBullet;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_EFFECT, TEXT("Prototype_GameObject_Soldier_Bomb"), nullptr, &pBullet);
		pBullet->Set_Owner(m_pOwner.lock());
		pBullet->Get_TransformCom().lock()->Set_Position(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

void CSoldier_Grenade::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

void CSoldier_Grenade::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CSoldier_Grenade::Render()
{
	if (m_pModelCom)
	{
		if (FAILED(Bind_ShaderResources()))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			// 텍스처 바인딩
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

			// 패스, 버퍼 바인딩
			if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
				RETURN_EFAIL;

			//버퍼 렌더
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}
	return S_OK;
}

HRESULT CSoldier_Grenade::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, TEXT("Soldier_Grenade"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CSoldier_Grenade::Bind_ShaderResources()
{
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pModelCom->Calculate_TransformFloat4x4FromParent())))
		RETURN_EFAIL;

	return S_OK;
}

void CSoldier_Grenade::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Grenade::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Grenade::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Grenade::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CSoldier_Grenade::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CSoldier_Grenade::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CSoldier_Grenade::Free()
{
	__super::Free();
}
