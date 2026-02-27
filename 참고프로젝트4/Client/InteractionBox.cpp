#include "stdafx.h"
#include "InteractionBox.h"
#include "MapObject.h"
#include "GameInstance.h"
#include "InteractionBox_Split.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"

#include "UI_Manager.h"

IMPLEMENT_CREATE(CInteractionBox)
IMPLEMENT_CLONE(CInteractionBox, CGameObject)


CInteractionBox::CInteractionBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CDynamicObject(pDevice, pContext)
{

}

CInteractionBox::CInteractionBox(const CInteractionBox& rhs)
	: CDynamicObject(rhs)
{
}

HRESULT CInteractionBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractionBox::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC pDesc = {};

	if (FAILED(__super::Initialize(&pDesc)))
		RETURN_EFAIL;

	if (pArg != nullptr)
	{
		CMapObject::MODEL_DESC* pDesc = (CMapObject::MODEL_DESC*)pArg;

		m_szModelTag = pDesc->strModelTag;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vInitialPosition);
	}

	return S_OK;
}

void CInteractionBox::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};

	_float3 vMaterial = _float3(0.7f, 0.7f, 0.3f);
	_float fMass = 10000000.f;

	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MAP_DYNAMIC, m_pTransformCom, _float3(0.62f,0.62f,0.62f), false, nullptr, false, vMaterial, fMass);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	m_vPhysXColliderLocalOffset = { 0.f,-0.31f,0.f };

}

void CInteractionBox::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CInteractionBox::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CInteractionBox::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CInteractionBox::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

void CInteractionBox::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CInteractionBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CInteractionBox::Ready_Components(void* pArg)
{
	if (m_szModelTag == L"Box3")
		m_eBoxType = METAL;
	else if (m_szModelTag == L"Box4")
		m_eBoxType = WOOD;

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

HRESULT CInteractionBox::Bind_ShaderResources()
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

void CInteractionBox::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	if(pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_ATTACK)
	{
		PxContactPairPoint* pContactPoint = new PxContactPairPoint[ContactInfo.contactCount];
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint, ContactInfo.contactCount);

		CInteractionBox_Split::BOX_SPLIT_DESC SplitDesc;
		vector<_float3> vecForce;
		vector<_int> vecIndex;
		shared_ptr<CInteractionBox_Split> pSplit = nullptr;

		_uint iCount = m_eBoxType == METAL ? 6 : Random({6,8,10});
		SplitDesc.eBoxType = (CInteractionBox_Split::BOX_TYPE)m_eBoxType;

		m_pGameInstance->RandomFloat3_Vector(vecForce, iCount, Convert_Vector(pContactPoint[0].normal) + _float3(-0.2f, 0.f, -0.2f), Convert_Vector(pContactPoint[0].normal) + _float3(0.2f, 0.f, 0.2f));
		if(m_eBoxType == WOOD)
			m_pGameInstance->RandomInt_Vector(vecIndex, iCount,0,21);

		for (_uint i = 0; i < iCount; ++i)
		{
			SplitDesc.vForce = XMVector3Normalize(vecForce[i]) * 80;
			if (m_eBoxType == METAL)
				SplitDesc.iBoxIndex = i;
			else
				SplitDesc.iBoxIndex = vecIndex[i];

			m_pGameInstance->Add_CloneObject<CInteractionBox_Split>(m_pGameInstance->Get_CreateLevelIndex(),L_ENVIRONMENT, L"Prototype_GameObject_InteractionBox_Split",&SplitDesc, &pSplit);
			if(pSplit)
				pSplit->Get_TransformCom().lock()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION)+ m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_vPhysXColliderLocalOffset.y);
		}
		//GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Box_Particle", shared_from_this(), CEffect::USE_FOLLOW_NORMAL,
		//	_float3(0.f, 0.31f, 0.f));
		//UI
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<_int> distribution(1, 3);
		uniform_int_distribution<_int> distributionbool(0, 1);

		_int RandomGIL = distribution(gen);
		_bool IsGetRandom = distributionbool(gen);

		if (true == IsGetRandom)
		{
			GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(1, RandomGIL);
			GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(3);
		}

		Set_Dead();
	}
}

void CInteractionBox::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CInteractionBox::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CInteractionBox::Write_Json(json& Out_Json)
{
	Out_Json["ModelTag"] = WstrToStr(m_szModelTag);

	__super::Write_Json(Out_Json);
}

void CInteractionBox::Load_Json(const json& In_Json)
{
	m_szModelTag = StrToWstr(In_Json["ModelTag"]);

	Ready_Components(nullptr);

	__super::Load_Json(In_Json);
}

void CInteractionBox::Free()
{
	__super::Free();
}