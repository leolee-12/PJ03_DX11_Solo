#include "Body.h"

#include "GameInstance.h"
#include "Player.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody::CBody(const CBody& Prototype)
    : CPartObject{ Prototype }
{
}

const _float4x4* CBody::Get_SocketBoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_SocketBoneMatrix_Ptr(pBoneName);
}

HRESULT CBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_Animation(3, false);

    return S_OK;
}

void CBody::Priority_Update(_float fTimeDelta)
{
}

void CBody::Update(_float fTimeDelta)
{

    if (*m_pParentState & CPlayer::STATE::IDLE)
    {
        m_pModelCom->Set_Animation(3, true);
    }

    if (*m_pParentState & CPlayer::STATE::RUN)
    {
        m_pModelCom->Set_Animation(4, true);
    }

    m_pModelCom->Play_Animation(fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBody::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBody::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint       iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT CBody::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Shadow_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Shadow_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;

    _uint       iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices");

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody::Ready_Components()
{
    /* For.Com_Model */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    /* For. Com_Collider */
    CBounding_Sphere::SPHERE_DESC     ColliderDesc{};
    ColliderDesc.fRadius = 0.2f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.9f, 0.f);

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CBody::Bind_ShaderResources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;


    return S_OK;
}


CGameObject* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody* pInstance = new CBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CBody::Clone(void* pArg)
{
    CBody* pInstance = new CBody(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBody::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);

}
