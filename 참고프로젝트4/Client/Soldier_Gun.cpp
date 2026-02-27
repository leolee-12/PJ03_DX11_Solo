#include "stdafx.h"
#include "Soldier_Gun.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CSoldier_Gun)
IMPLEMENT_CLONE(CSoldier_Gun, CGameObject)

CSoldier_Gun::CSoldier_Gun(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pDevice, pContext)
{
}

CSoldier_Gun::CSoldier_Gun(const CSoldier_Gun& rhs)
    : BASE(rhs)
{
}

HRESULT CSoldier_Gun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoldier_Gun::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CSoldier_Gun::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    m_pTransformCom->Rotation(0.f, XMConvertToRadians(90.f), 0.f);
}

void CSoldier_Gun::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CSoldier_Gun::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CSoldier_Gun::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CSoldier_Gun::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this())))
        return;
}

void CSoldier_Gun::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CSoldier_Gun::Render()
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

HRESULT CSoldier_Gun::Ready_Components(void* pArg)
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
        TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
        RETURN_EFAIL;

    if (m_pModelCom)
    {
        m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, TEXT("Soldier_Gun"));
        if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
            RETURN_EFAIL;
    }

    return S_OK;
}

HRESULT CSoldier_Gun::Bind_ShaderResources()
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

void CSoldier_Gun::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Gun::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Gun::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CSoldier_Gun::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CSoldier_Gun::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CSoldier_Gun::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CSoldier_Gun::Free()
{
    __super::Free();
}
