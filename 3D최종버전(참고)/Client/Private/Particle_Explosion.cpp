#include "Particle_Explosion.h"
#include "GameInstance.h"

CParticle_Explosion::CParticle_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CParticle_Explosion::CParticle_Explosion(const CParticle_Explosion& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CParticle_Explosion::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Explosion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    

    return S_OK;
}

void CParticle_Explosion::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CParticle_Explosion::Update(_float fTimeDelta)
{

    m_pVIBufferCom->Spread(fTimeDelta);
 
    __super::Update(fTimeDelta);
}

void CParticle_Explosion::Late_Update(_float fTimeDelta)
{  
    
   
    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this);
    
    __super::Late_Update(fTimeDelta);
}

HRESULT CParticle_Explosion::Render()
{
   /* */

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;   

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CParticle_Explosion::Ready_Components()
{     
    /* For.Com_VIBuffer */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
  
    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxPosInstanceParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CGameObject* CParticle_Explosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Explosion* pInstance = new CParticle_Explosion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CParticle_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CParticle_Explosion::Clone(void* pArg)
{
    CParticle_Explosion* pInstance = new CParticle_Explosion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Explosion::Free()
{
    __super::Free();


    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

}
