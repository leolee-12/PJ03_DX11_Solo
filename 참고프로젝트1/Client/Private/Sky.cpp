#include "Sky.h"
#include "GameInstance.h"



CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CSky::CSky(const CSky& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CSky::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSky::Update(_float fTimeDelta)
{
 
    __super::Update(fTimeDelta);
}

void CSky::Late_Update(_float fTimeDelta)
{  
    
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
    
    __super::Late_Update(fTimeDelta);
}

HRESULT CSky::Render()
{
   /* */

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;   

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSky::Ready_Components()
{     
    /* For.Com_VIBuffer */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
  
    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CGameObject* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSky* pInstance = new CSky(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSky");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CSky::Clone(void* pArg)
{
    CSky* pInstance = new CSky(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSky");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSky::Free()
{
    __super::Free();


    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

}
