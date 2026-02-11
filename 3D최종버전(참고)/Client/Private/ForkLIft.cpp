#include "ForkLift.h"
#include "GameInstance.h"

CForkLift::CForkLift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CForkLift::CForkLift(const CForkLift& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CForkLift::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CForkLift::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_pGameInstance->Random(0.f, 10.f),
        2.0f,
        m_pGameInstance->Random(0.f, 10.f),
        1.f
    ));

    m_pTransformCom->Rotation(
        0.f,
        m_pGameInstance->Random(XMConvertToRadians(0.f), XMConvertToRadians(360.f)), 
        0.f    
    );

    

    return S_OK;
}

void CForkLift::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    
}

void CForkLift::Update(_float fTimeDelta)
{
    
    __super::Update(fTimeDelta);
}

void CForkLift::Late_Update(_float fTimeDelta)
{      
    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */

    if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(STATE::POSITION), 2.f))
    {
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
    }

    __super::Late_Update(fTimeDelta);
}

HRESULT CForkLift::Render()
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
        if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }




    return S_OK;
}

HRESULT CForkLift::Ready_Components()
{     
    /* For.Com_Model */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
            
    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    

    return S_OK;
}

CGameObject* CForkLift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CForkLift* pInstance = new CForkLift(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CForkLift");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CForkLift::Clone(void* pArg)
{
    CForkLift* pInstance = new CForkLift(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CForkLift");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CForkLift::Free()
{
    __super::Free();

    
    
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
