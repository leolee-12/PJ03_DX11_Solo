#include "stdafx.h"
#include "Light_Cone.h"

#include "Transform.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CLight_Cone)
IMPLEMENT_CLONE(CLight_Cone, CGameObject)

CLight_Cone::CLight_Cone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CLight_Cone::CLight_Cone(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLight_Cone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight_Cone::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC* pDesc = (GAMEOBJECT_DESC*)pArg;

	if (FAILED(__super::Initialize(&pDesc)))
		RETURN_EFAIL;

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vInitialPosition);
	}

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	m_pModelCom->Transform().Set_Scaling(1000.f, 1000.f, 1000.f);

	return S_OK;
}

void CLight_Cone::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CLight_Cone::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CLight_Cone::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLight_Cone::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CLight_Cone::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

void CLight_Cone::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CLight_Cone::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;


	return S_OK;
}

HRESULT CLight_Cone::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, L"LightCone");
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;

		m_pModelCom->Transform().Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f));
	}

	return S_OK;
}

HRESULT CLight_Cone::Bind_ShaderResources()
{
#pragma TODO_MSG(학준이가 알려줌)

	if (m_pModelCom)
	{
		//if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
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

			if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_LightConeColor", &vColor, sizeof(float) * 4)))
				RETURN_EFAIL;

			if (FAILED(m_pModelCom->ShaderComp()->Begin(5)))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}

	return S_OK;
}

void CLight_Cone::Free()
{
	__super::Free();
}
