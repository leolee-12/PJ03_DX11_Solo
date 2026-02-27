#include "Monster.h"
#include "GameInstance.h"



CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_pGameInstance->Random(0.f, 10.f),
        1.0f,
        m_pGameInstance->Random(0.f, 10.f),
        1.f
    ));

    m_pModelCom->Set_Animation(0, false);

    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
    m_pModelCom->Set_Animation(0, true);

    if (true == m_pModelCom->is_AnimFinished())
        int a = 10;


    m_pModelCom->Play_Animation(fTimeDelta);

    for (size_t i = 0; i < 3; i++)
    {
        m_pColliderCom[i]->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
    }

    Intersect_ToPlayer();
 
    __super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{  
    
    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */

    if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(STATE::POSITION), 1.f))
    {
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
        for (size_t i = 0; i < 3; i++)
        {
            m_pGameInstance->Add_DebugComponent(m_pColliderCom[i]);
        }
#endif

    }
        
    __super::Late_Update(fTimeDelta);
}

HRESULT CMonster::Render()
{
   /* */

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;   

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;

    _uint       iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");
        //if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
        //    return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }



    return S_OK;
}

HRESULT CMonster::Ready_Components()
{     
    /* For.Com_Model */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
    

    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;


    /* For. Com_Collider_AABB */
    CBounding_AABB::AABB_DESC     AABBDesc{};
    AABBDesc.vSize = _float3(0.8f, 1.2f, 0.8f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[0]), &AABBDesc)))
        return E_FAIL;

    /* For. Com_Collider_Sphere */
    CBounding_Sphere::SPHERE_DESC     ShpereDesc{};
    ShpereDesc.fRadius = 0.2f;
    ShpereDesc.vCenter = _float3(0.f, ShpereDesc.fRadius + 0.9f, 0.f);

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[1]), &ShpereDesc)))
        return E_FAIL;

    /* For. Com_Collider_OBB */
    CBounding_OBB::OBB_DESC     OBBDesc{};
    OBBDesc.vSize = _float3(0.8f, 0.8f, 0.8f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * 0.5f, 0.f);
    OBBDesc.vRadians = _float3(0.f, XMConvertToRadians(45.0f), 0.f);

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom[2]), &OBBDesc)))
        return E_FAIL;


    return S_OK;
}

_bool CMonster::Intersect_ToPlayer()
{
    CCollider*  pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), 0, TEXT("Com_Collider")));
    if (nullptr == pTargetCollider)
        return false;

    return m_pColliderCom[2]->Intersect(pTargetCollider);    
}

CGameObject* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster* pInstance = new CMonster(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CMonster::Clone(void* pArg)
{
    CMonster* pInstance = new CMonster(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster::Free()
{
    __super::Free();

    for (size_t i = 0; i < 3; i++)
    {
        Safe_Release(m_pColliderCom[i]);
    }

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
