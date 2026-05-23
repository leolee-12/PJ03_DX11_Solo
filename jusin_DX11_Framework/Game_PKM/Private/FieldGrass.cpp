#include "FieldGrass.h"

#include "GameInstance.h"

namespace
{
	static constexpr _uint PASS_GRASS = 7;
}

CFieldGrass::CFieldGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = L"FieldGrass";
}

CFieldGrass::CFieldGrass(const CFieldGrass& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CFieldGrass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFieldGrass::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_tDesc = *static_cast<FIELDGRASS_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->ScaleTo(m_tDesc.fScale, m_tDesc.fScale, m_tDesc.fScale);
	m_pTransformCom->Rotation(0.f, m_tDesc.fYaw, 0.f);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_tDesc.vSpawnPos.x, m_tDesc.vSpawnPos.y, m_tDesc.vSpawnPos.z, 1.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(m_RenderProfile.Build(m_pModelCom, m_tDesc.pRenderRule)))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMaterials(); ++i)
		m_RenderProfile.Set_Pass(i, PASS_GRASS);

	return S_OK;
}

void CFieldGrass::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CFieldGrass::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    vector<CRenderProfile::MATERIAL_SLOT> Slots =
    {
            { MATERIAL_TYPE::DIFFUSE, "g_TexDiff", 0 },
    };

    if (FAILED(m_RenderProfile.Bind_AndDraw(m_pShaderCom, Slots)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFieldGrass::Ready_Components()
{
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_MAP,
        COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(m_tDesc.iModelLevelIndex, m_tDesc.strModelTag,
        COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFieldGrass::Bind_ShaderResources()
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

    return S_OK;
}

CFieldGrass* CFieldGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFieldGrass* pInstance = new CFieldGrass(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CFieldGrass");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFieldGrass::Clone(void* pArg)
{
    CFieldGrass* pInstance = new CFieldGrass(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CFieldGrass");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFieldGrass::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}