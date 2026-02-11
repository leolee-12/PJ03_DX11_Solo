#include "Sprite.h"
#include "GameInstance.h"

CSprite::CSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CSprite::CSprite(const CSprite& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CSprite::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSprite::Initialize(void* pArg)
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

    

    return S_OK;
}

void CSprite::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    
}

void CSprite::Update(_float fTimeDelta)
{
    m_fFrame += 90.f * fTimeDelta;

    if (m_fFrame >= 90.0f)
        m_fFrame = 0.f;

    __super::Update(fTimeDelta);
}

void CSprite::Late_Update(_float fTimeDelta)
{      
    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CSprite::Render()
{
   /* */

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;   

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4_Ptr(D3DTS::PROJECTION))))
        return E_FAIL;

    

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();



    return S_OK;
}

HRESULT CSprite::Ready_Components()
{     
    /* For.Com_VIBuffer */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Explosion"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
        
    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    

    return S_OK;
}

CGameObject* CSprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSprite* pInstance = new CSprite(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSprite");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CSprite::Clone(void* pArg)
{
    CSprite* pInstance = new CSprite(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSprite");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSprite::Free()
{
    __super::Free();

    
    Safe_Release(m_pTextureCom);    
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

}
