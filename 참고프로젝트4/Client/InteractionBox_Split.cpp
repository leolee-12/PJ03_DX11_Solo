#include "stdafx.h"
#include "InteractionBox_Split.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CInteractionBox_Split)
IMPLEMENT_CLONE(CInteractionBox_Split, CGameObject)

vector<wstring> CInteractionBox_Split::m_szBoxModelTags[2] = {
	{
		L"TreasureMetalBox_01_A_Physics",
		L"TreasureMetalBox_01_B_Physics",
		L"TreasureMetalBox_01_C_Physics",
		L"TreasureMetalBox_01_D_Physics",
		L"TreasureMetalBox_01_E_Physics",
		L"TreasureMetalBox_01_F_Physics"
	},
	{
		L"TreasureWoodBox_01_A_Physics",
		L"TreasureWoodBox_01_B_Physics",
		L"TreasureWoodBox_01_C_Physics",
		L"TreasureWoodBox_01_D_Physics",
		L"TreasureWoodBox_01_E_Physics",
		L"TreasureWoodBox_01_F_Physics",
		L"TreasureWoodBox_01_G_Physics",
		L"TreasureWoodBox_01_H_Physics",
		L"TreasureWoodBox_01_I_Physics",
		L"TreasureWoodBox_01_J_Physics",
		L"TreasureWoodBox_01_K_Physics",
		L"TreasureWoodBox_01_L_Physics",
		L"TreasureWoodBox_01_M_Physics",
		L"TreasureWoodBox_01_N_Physics",
		L"TreasureWoodBox_01_O_Physics",
		L"TreasureWoodBox_01_P_Physics",
		L"TreasureWoodBox_01_Q_Physics",
		L"TreasureWoodBox_01_R_Physics",
		L"TreasureWoodBox_01_S_Physics",
		L"TreasureWoodBox_01_T_Physics",
		L"TreasureWoodBox_01_U_Physics",
		L"TreasureWoodBox_01_V_Physics"
	}
};


CInteractionBox_Split::CInteractionBox_Split(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CDynamicObject(pDevice, pContext)
{
}

CInteractionBox_Split::CInteractionBox_Split(const CInteractionBox_Split& rhs)
	:CDynamicObject(rhs)
{
}

HRESULT CInteractionBox_Split::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractionBox_Split::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&pArg)))
		RETURN_EFAIL;

	BOX_SPLIT_DESC Desc = *(BOX_SPLIT_DESC*)pArg;
	m_eBoxType = Desc.eBoxType;
	m_iBoxIndex = Desc.iBoxIndex;
	m_vForce = Desc.vForce;

	m_szModelTag = m_szBoxModelTags[m_eBoxType][m_iBoxIndex];

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;


	return S_OK;
}

void CInteractionBox_Split::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	
	PHYSXCOLLIDER_DESC ColliderDesc = {};

	_float3 vMaterial = _float3(0.7f, 0.7f, 0.3f);
	_float fMass = 30000000.f;
	_float fScaleY = m_eBoxType == METAL ? 0.62f : 0.3f;
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MAP_DYNAMIC_SPLIT, m_pTransformCom, _float3(0.05f, fScaleY, 0.62f), false, nullptr, false, vMaterial, fMass);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	m_pModelCom->Transform().Set_Scaling(0.4f, 0.4f, 0.4f);
	m_pPhysXColliderCom->Add_Force(m_vForce);
}

void CInteractionBox_Split::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CInteractionBox_Split::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
}

void CInteractionBox_Split::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5.f <= m_fTimeAcc)
	{
		Set_Dead();
	}
}

void CInteractionBox_Split::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

void CInteractionBox_Split::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CInteractionBox_Split::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CInteractionBox_Split::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, m_szModelTag);
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;
	}
	return S_OK;
}

HRESULT CInteractionBox_Split::Bind_ShaderResources()
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

			if (FAILED(m_pModelCom->ShaderComp()->Begin(4)))
				RETURN_EFAIL;


			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}
}

void CInteractionBox_Split::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CInteractionBox_Split::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CInteractionBox_Split::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CInteractionBox_Split::Write_Json(json& Out_Json)
{
}

void CInteractionBox_Split::Load_Json(const json& In_Json)
{
}

void CInteractionBox_Split::Free()
{
	__super::Free();
}
