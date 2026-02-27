#include "stdafx.h"
#include "Soldier_Tonfa.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CSoldier_Tonfa)
IMPLEMENT_CLONE(CSoldier_Tonfa, CGameObject)

CSoldier_Tonfa::CSoldier_Tonfa(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CSoldier_Tonfa::CSoldier_Tonfa(const CSoldier_Tonfa& rhs)
    : BASE(rhs)
{
}

HRESULT CSoldier_Tonfa::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoldier_Tonfa::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CSoldier_Tonfa::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    //Transform에 기록된 크기, 위치, 회전값으로 BOX 생성
    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_PLAYER_ATTACK, m_pTransformCom, _float3(1.6f, 0.3f, 0.1f), false, nullptr, true);
    m_vPhysXColliderLocalOffset = { 0.8f,0.0f,0.0f };

    if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;

    m_pTransformCom->Rotation(0.f, PI * 0.5f, 0.f);

    Set_SkillName("Soldier_Tonfa");
}

void CSoldier_Tonfa::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CSoldier_Tonfa::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CSoldier_Tonfa::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CSoldier_Tonfa::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;
}

void CSoldier_Tonfa::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CSoldier_Tonfa::Render()
{
    if (m_pModelCom)
    {
        if (FAILED(Bind_ShaderResources()))
            RETURN_EFAIL;

        auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
        for (_uint i = 0; i < ActiveMeshes.size(); ++i)
        {
            // 뼈 바인딩
            //if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
              //  RETURN_EFAIL;

            // 텍스처 바인딩
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
                RETURN_EFAIL;
            if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
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

HRESULT CSoldier_Tonfa::Ready_Components(void* pArg)
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
        TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
        RETURN_EFAIL;

    if (m_pModelCom)
    {
        m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, TEXT("Soldier_Tonfa"));
        if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
            RETURN_EFAIL;
    }

    return S_OK;
}

HRESULT CSoldier_Tonfa::Bind_ShaderResources()
{
    // 공통 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
        RETURN_EFAIL;

    // 월드 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;

    return S_OK;
}

void CSoldier_Tonfa::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Tonfa::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Tonfa::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Tonfa::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Tonfa::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Tonfa::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CSoldier_Tonfa::Free()
{
    __super::Free();
}
