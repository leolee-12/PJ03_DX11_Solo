#include "BackGround.h"
#include "GameInstance.h"



CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CBackGround::CBackGround(const CBackGround& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    BACKGROUND_DESC     Desc{};

    Desc.fX = 150.0f;
    Desc.fY = 150.0f;
    Desc.fSizeX = 300.0f;
    Desc.fSizeY = 300.0f;

    Desc.fSpeedPerSec = 5.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CBackGround::Update(_float fTimeDelta)
{
    m_fX += 5.0f * fTimeDelta;

    __super::Update(fTimeDelta);
}

void CBackGround::Late_Update(_float fTimeDelta)
{  
    
    /* 렌더러의 그룹들 중 어떤 순서로 그려져야할지 적절한 위치에 추가해준다. */
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
    
    __super::Late_Update(fTimeDelta);
}

HRESULT CBackGround::Render()
{
   /* */

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;   

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBackGround::Ready_Components()
{     
    /* For.Com_VIBuffer */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CGameObject* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBackGround* pInstance = new CBackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

}
