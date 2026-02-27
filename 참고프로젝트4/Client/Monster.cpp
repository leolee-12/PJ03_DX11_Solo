#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CMonster)
IMPLEMENT_CLONE(CMonster, CGameObject)

CMonster::CMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCharacter(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	:CCharacter(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	m_iShaderPassIndex = 0;
	m_strDissolveTextureTag = L"DissolveExplode_Fractal01_M";
	// 디졸브 텍스쳐 필요시 각 클래스에서 변경. 현재는 공용

	return S_OK;
}

void CMonster::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

}

void CMonster::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CMonster::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Dissolve(fTimeDelta);
}

void CMonster::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CMonster::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CMonster::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CMonster::Render()
{
	auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
	for (_uint i = 0; i < ActiveMeshes.size(); ++i)
	{
		// 뼈 바인딩
		if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
			RETURN_EFAIL;

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
		if (FAILED(m_pModelCom->ShaderComp()->Begin(m_iShaderPassIndex)))
			RETURN_EFAIL;
		//버퍼 렌더
		if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CMonster::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	// 공통 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	// 월드 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		RETURN_EFAIL;

	// Dissolve
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pModelCom->ShaderComp().get(), "g_DissolveTexture", MATERIALTYPE::MATERIATYPE_DISSOLVE, 0, true))) RETURN_EFAIL;

	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmout, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fDissolveGradiationDistance", &m_fDissolveGradiationDistance, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDissolveGradiationStartColor", &m_vDissolveGradiationStartColor, sizeof(_float3)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDissolveGradiationEndColor", &m_vDissolveGradiationEndColor, sizeof(_float3)))) RETURN_EFAIL;
	

	return S_OK;
}

void CMonster::Load_Json(const json& In_Json)
{
	__super::Load_Json(In_Json);

#if LEVEL_STAGE1_TEST_MAP == 0
	TurnOff_State(OBJSTATE::Active);

#elif LEVEL_STAGE1_TEST_MAP == 98
	TurnOn_State(OBJSTATE::Active);
#endif
}

void CMonster::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CMonster::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CMonster::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CMonster::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CMonster::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CMonster::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CMonster::Free()
{
	__super::Free();
}