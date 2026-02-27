#include "stdafx.h"
#include "Instance_Object.h"
#include "GameInstance.h"
#include "Model_Inst.h"

IMPLEMENT_CREATE(CInstance_Object)
IMPLEMENT_CLONE(CInstance_Object, CGameObject)
CInstance_Object::CInstance_Object(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

CInstance_Object::CInstance_Object(const CInstance_Object& rhs)
	:CGameObject(rhs)
	, m_pInstanceModel(rhs.m_pInstanceModel)
{
}

HRESULT CInstance_Object::Initialize_Prototype()
{
	m_pInstanceModel = CModel_Inst::Create(m_pDevice, m_pContext);
	return S_OK;
}

HRESULT CInstance_Object::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC Desc = {};

	if (FAILED(__super::Initialize(&Desc)))
		RETURN_EFAIL;

	if (pArg != nullptr)
	{
		INSTANCE_OBJECT_DESC* pDesc = (INSTANCE_OBJECT_DESC*)pArg;

		m_szModelTag = StrToWstr(pDesc->strName);
		
	}

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CInstance_Object::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CInstance_Object::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CInstance_Object::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CInstance_Object::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CInstance_Object::Before_Render(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

	__super::Before_Render(fTimeDelta);
}

void CInstance_Object::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CInstance_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	return S_OK;
}


HRESULT CInstance_Object::Ready_Components(void* pArg)
{
	INSTANCE_OBJECT_DESC* pDesc = ((INSTANCE_OBJECT_DESC*)pArg);
	m_szModelTag = StrToWstr(pDesc->strName);
	_matrix matLocal = m_pTransformCom->Get_WorldMatrixInverse();
	vector<float4x4> vecTemp;
	if (!m_szModelTag.empty())
	{
		for (auto& iter : pDesc->vecInstanceVertex)
		{
			_matrix matTemp = XMLoadFloat4x4(&iter) * matLocal;
			_float4x4 matDst;
			XMStoreFloat4x4(&matDst, matTemp);
			vecTemp.push_back(matDst);
		}
		pDesc->vecInstanceVertex = vecTemp;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Model_Inst"),
			TEXT("Com_Inst"), &(m_pInstanceModel), pDesc)))
			RETURN_EFAIL;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
			TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
			RETURN_EFAIL;

		if (m_pModelCom)
		{
			m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, m_szModelTag);
			if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model_Instnace", VTX_MESH_INSTANCE::Elements, VTX_MESH_INSTANCE::iNumElements)))
				RETURN_EFAIL;
		}
	}
	return S_OK;
}

HRESULT CInstance_Object::Bind_ShaderResources()
{
	if (m_pModelCom)
	{
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
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

			if (FAILED(m_pInstanceModel->Render(i)))
				RETURN_EFAIL;
		}
	}

	return S_OK;
}

void CInstance_Object::Free()
{
	__super::Free();
}

