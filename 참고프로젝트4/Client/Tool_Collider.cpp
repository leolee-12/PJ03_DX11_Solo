#include "stdafx.h"
#include "Tool_Collider.h"
#include "GameInstance.h"
#include "PhysX_Collider.h"
#include "PhysX_Manager.h"

IMPLEMENT_CREATE(CTool_Collider)
IMPLEMENT_CLONE(CTool_Collider, CGameObject)

CTool_Collider::CTool_Collider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CTool_Collider::CTool_Collider(const CTool_Collider& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTool_Collider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

    return S_OK;
}

HRESULT CTool_Collider::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC Desc = {};

	TurnOn_State(OBJSTATE::Active);
	TurnOn_State(OBJSTATE::Begin_Play);

	if (FAILED(__super::Initialize(&Desc)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL; 

	COLLDESC* pDesc = (COLLDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	return S_OK;
}

void CTool_Collider::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CTool_Collider::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CTool_Collider::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTool_Collider::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugRender(m_pColliderCom);
//#endif	
}

void CTool_Collider::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CTool_Collider::Render()
{
	if (m_pModelCom)
	{
		_float4x4 TempFloat4x4 = m_pTransformCom->Get_WorldFloat4x4();

		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &TempFloat4x4)))
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

HRESULT CTool_Collider::Ready_Components(void* pArg)
{
	CBounding_OBB::BOUNDING_OBB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BoundingDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	BoundingDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Bounding"), &m_pColliderCom, &BoundingDesc)))
		RETURN_EFAIL;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, L"CST0000_01_TransparentBOX");
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;
	}

	return S_OK;
}


void CTool_Collider::Free()
{
	__super::Free();
}
