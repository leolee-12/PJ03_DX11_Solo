#include "Terrain.h"
#include "GameInstance.h"



CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);



    m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CTerrain::Update(_float fTimeDelta)
{
    
    
    m_pVIBufferCom->Culling(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
 
    __super::Update(fTimeDelta);
}

void CTerrain::Late_Update(_float fTimeDelta)
{  
    
    
    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
#ifdef _DEBUG    
    m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
    __super::Late_Update(fTimeDelta);
}

HRESULT CTerrain::Render()
{
   /* */

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;   

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;

    

    if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pTextureCom[TEXTURE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();



    return S_OK;
}

HRESULT CTerrain::Ready_Components()
{     
    /* For.Com_VIBuffer */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]), nullptr)))
        return E_FAIL;

    /* For.Com_Texture_Mask */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain_Mask"),
        TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]), nullptr)))
        return E_FAIL;

    /* For.Com_Texture_Brush */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain_Brush"),
        TEXT("Com_Texture_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_BRUSH]), nullptr)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    /* For. Com_Navigation */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), nullptr)))
        return E_FAIL;

    

    return S_OK;
}

CGameObject* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = new CTerrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();

    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

}
