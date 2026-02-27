#include "stdafx.h"
#include "..\Public\MapObject.h"
#include "CommonModelComp.h"
#include "GameInstance.h"
#include "PhysX_Collider.h"
#include "PhysX_Manager.h"

IMPLEMENT_CREATE(CMapObject)
IMPLEMENT_CLONE(CMapObject, CGameObject)

CMapObject::CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{

}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC pDesc = {};
	//pDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Initialize(&pDesc)))
		RETURN_EFAIL;

	if (pArg != nullptr)
	{
		MODEL_DESC* pDesc = (MODEL_DESC*)pArg;

		m_szModelTag = pDesc->strModelTag;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vInitialPosition);
	}
	else
		m_szModelTag = L"BuildingClockTower_01_A_1stAvenue";

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_STATIC, shared_from_this())))
		RETURN_EFAIL;

	return S_OK;
}

void CMapObject::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	if (m_szModelTag == L"Light_27_1stAvenue")
	{
		m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), XMConvertToRadians(-90));
	}
	if (m_szModelTag == L"Light_18_1stAvenue")
	{
		m_pTransformCom->Set_Scaling(0.4f, 0.4f, 0.4f);
	}

	//런타임 중에 스케일링 불가능 -> Begin_Play에서 콜라이더 생성
	_float3 vScale;
	_float3 vMaterial = _float3(0.7f, 0.7f, 0.3f);
	_float fMass = 60000000.f;

	if (m_szModelTag == L"BallLight_01_Emissive_5thAvenue")
	{
		m_pTransformCom->Set_Scaling(2.f, 2.f, 2.f);
	}

	if (m_strObjectTag == L"TestModel")
	{
		m_ModelType = NONE;
	}

	if (m_ModelType == BOX)
	{
		if (m_szModelTag == L"CardboardBox_01_Physics" || m_szModelTag == L"CardboardBox_02_Physics" || m_szModelTag == L"CardboardBox_03_Physics")
		{
			vScale = _float3(0.5f, 0.5f, 0.6f);
			m_vPhysXColliderLocalOffset = { 0.f,-0.2f,0.f };
		}
		else if (m_szModelTag == L"Barricade_02_Physics")
		{
			vScale = _float3(0.4f, 0.8f, 1.1f);
			m_vPhysXColliderLocalOffset = { 0.f,-0.4f,0.f };

		}
		else if (m_szModelTag == L"Barricade_04_Physics")
		{
			vScale = _float3(1.1f, 1.f, 0.6f);
			m_vPhysXColliderLocalOffset = { 0.f,-0.5f,0.f };

		}
		else if (m_szModelTag == L"Bottlebox_01_Physics")
		{
			vScale = _float3(0.45f, 0.32f, 0.45f);
			m_vPhysXColliderLocalOffset = { -0.15f,-0.15f,-0.2f };
		}
		else if (m_szModelTag == L"Container_01_Physics")
		{
			vScale = _float3(0.45f, 0.35f, 0.56f);
			m_vPhysXColliderLocalOffset = { 0.f,-0.17f,0.f };

		}
		else if (m_szModelTag == L"SupportParts_01_Physics" || m_szModelTag == L"SupportParts_02_Physics")
		{
			vScale = _float3(0.7f, 0.6f, 1.f);
			m_vPhysXColliderLocalOffset = { 0.f,-0.3f,0.f };

		}
		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MAP_DYNAMIC, m_pTransformCom, vScale, false, nullptr, false, vMaterial, fMass);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;
	}
	//X가 Radius Y가 Height, 마지막인자 Mass(질량)값을 높이면 무거워짐 
	else if (m_ModelType == CYLINDER)
	{
		if (m_szModelTag == L"Barrel_01_Physics")
		{
			vScale = _float3(0.25f, 0.7f, 0.6f);
			m_vPhysXColliderLocalOffset = { -0.35f,0.f,0.f };
		}
		else
		{
			vScale = _float3(0.2f, 0.35f, 0.6f);
			//m_vPhysXColliderLocalOffset = { 0.f,-0.2f,-0.1f };
			m_vPhysXColliderLocalOffset = { -0.2f,0.f,0.f };
		}

		m_pModelCom->Transform().Rotation(_float3(0.f, 0.f, 1.f), XMConvertToRadians(-90.f));

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::CYLINDER, CL_MAP_DYNAMIC, m_pTransformCom, vScale, false, nullptr, false, vMaterial, fMass);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;
	}
	else if (m_ModelType == CONE)
	{
		if (m_szModelTag == L"SignBoard_04_Physics" || m_szModelTag == L"SignBoard_05_Physics")
		{
			vScale = _float3(0.5f, 0.8f, 1.f);
			m_vPhysXColliderLocalOffset = { -0.6f,0.f,0.f };

		}
		else
		{
			vScale = _float3(0.35f, 0.8f, 1.f);
			m_vPhysXColliderLocalOffset = { -0.4f,0.f,0.f };
		}
		m_pModelCom->Transform().Rotation(_float3(0.f, 0.f, 1.f), XMConvertToRadians(-90.f));
		m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

		PHYSXCOLLIDER_DESC ColliderDesc = {};
		PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::CONE, CL_MAP_DYNAMIC, m_pTransformCom, vScale, false, nullptr, false, vMaterial, fMass);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
			TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
			return;
	}

}

void CMapObject::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (m_pPhysXColliderCom)
	{
		m_pPhysXColliderCom->Synchronize_Transform(m_pTransformCom, m_vPhysXColliderLocalOffset, m_vPhysXColliderWorldOffset);
	}
}

void CMapObject::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMapObject::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CMapObject::Before_Render(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

	__super::Before_Render(fTimeDelta);

}

void CMapObject::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CMapObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CMapObject::Render_Shadow()
{
	if (m_pModelCom)
	{
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pModelCom->Calculate_TransformFloat4x4FromParent())))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLight_ViewMatrix())))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLight_ProjMatrix())))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{				
				if (FAILED(m_pModelCom->ShaderComp()->Begin(3)))
					RETURN_EFAIL;
				
				if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
					RETURN_EFAIL;
		}
	}

	return S_OK;
}

HRESULT CMapObject::Ready_Components(void* pArg)
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

HRESULT CMapObject::Bind_ShaderResources()
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

			if (m_iLightObjectType == NONE_LIGHTOBJ)
			{
				if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
					RETURN_EFAIL;
			}
			else
			{
				if (FAILED(m_pModelCom->ShaderComp()->Begin(4)))
					RETURN_EFAIL;
			}

			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}

	return S_OK;
}

HRESULT CMapObject::Change_ModelTag(wstring ModelTag)
{
	m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, ModelTag);
	return S_OK;
}

void CMapObject::Write_Json(json& Out_Json)
{
	Out_Json["ModelTag"] = WstrToStr(m_szModelTag);
	Out_Json["Model_Type"] = m_ModelType;
	Out_Json["bisLightObject"] = (_int)m_iLightObjectType;

	__super::Write_Json(Out_Json);
}

void CMapObject::Load_Json(const json& In_Json)
{
	m_szModelTag = StrToWstr(In_Json["ModelTag"]);
	m_ModelType = In_Json["Model_Type"];
	if (In_Json.contains("bisLightObject"))
	{
		m_iLightObjectType = (LIGHT_OBJECT_TYPE)In_Json["bisLightObject"];
	}

	m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, m_szModelTag);

	__super::Load_Json(In_Json);
}

void CMapObject::Free()
{
	__super::Free();
}
