#include "WaterPlane.h"
#include "GameInstance.h"
#include "VIBuffer_XZPlane.h"

CWaterPlane::CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
    m_strName = L"WaterPlane_Default";
}

CWaterPlane::CWaterPlane(const CWaterPlane& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CWaterPlane::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWaterPlane::Initialize(void* pArg)
{
    if (nullptr != pArg)
        m_tDesc = *static_cast<WATER_PLANE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(m_tDesc.vSpawnPos.x, m_tDesc.vSpawnPos.y, m_tDesc.vSpawnPos.z, 1.f));

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CWaterPlane::Priority_Update(_float fTimeDelta)
{
}

void CWaterPlane::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta * m_tDesc.fTimeScale;
}
void CWaterPlane::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CWaterPlane::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;
    return m_pVIBufferCom->Render();
}

HRESULT CWaterPlane::Ready_Components()
{
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_WATER,
        COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CVIBuffer_XZPlane::XZ_PLANE_DESC tPlaneDesc{};
    tPlaneDesc.fWidth = m_tDesc.fWidth * m_tDesc.fScale;
    tPlaneDesc.fDepth = m_tDesc.fDepth * m_tDesc.fScale;
    tPlaneDesc.fTileU = m_tDesc.fTileU;
    tPlaneDesc.fTileV = m_tDesc.fTileV;

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_XZPLANE,
        COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom), &tPlaneDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_WATER,
        COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWaterPlane::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
        return E_FAIL;

    if (m_pTextureCom->Get_NumTextures() < END)
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexWaterNet02", NET02)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexNorm", NORM)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexWaterNet01", NET01)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexWaterLight", LIGHT)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexSky", SKY)))
        return E_FAIL;

    return S_OK;
}

CWaterPlane* CWaterPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWaterPlane* pInstance = new CWaterPlane(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWaterPlane");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWaterPlane::Clone(void* pArg)
{
    CWaterPlane* pInstance = new CWaterPlane(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CWaterPlane");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWaterPlane::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    __super::Free();
}