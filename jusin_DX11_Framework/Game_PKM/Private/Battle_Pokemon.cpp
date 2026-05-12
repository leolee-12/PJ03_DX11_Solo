#include "Battle_Pokemon.h"
#include "Body_Pokemon.h"
#include "PokemonData_Manager.h"
#include "RenderRule_Manager.h"

CBattle_Pokemon::CBattle_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
    m_strName = { L"Pokemon_Default" };
}

CBattle_Pokemon::CBattle_Pokemon(const CBattle_Pokemon& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CBattle_Pokemon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBattle_Pokemon::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    const POKEMON_DESC* pDesc = static_cast<const POKEMON_DESC*>(pArg);

    if (nullptr == pDesc->pInstance)
        return E_FAIL;

    m_pInstance = pDesc->pInstance;
    m_iSide = pDesc->iSide;
    m_strBodyProtoTag = (0 != pDesc->strBodyProtoTag)
        ? pDesc->strBodyProtoTag
        : PROTO_OBJ_BODY_POKEMON;

    if (m_iSide >= g_kBattleSideCount)
        return E_FAIL;

    const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
    if (nullptr == pDataMgr)
        return E_FAIL;

    const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(m_pInstance->iSpeciesID);
    if (nullptr == pSpecies || 0 == pSpecies->strModelTag)
        return E_FAIL;

    m_strSpeciesModelTag = pSpecies->strModelTag;

    m_pRenderRule = pDesc->pRenderRule;
    if (nullptr == m_pRenderRule)
    {
        auto* pRuleManager = CRenderRule_Manager::GetInstance();
        if (nullptr == pRuleManager)
            return E_FAIL;

        m_pRenderRule = pRuleManager->Find_OrLoadMappingRule(pSpecies->pRenderMappingPath);

        if (nullptr == m_pRenderRule)
            m_pRenderRule = pRuleManager->Find_Rule(pSpecies->eRenderRuleKey);
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    const _float3& vPos = pDesc->vPos;
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->fYaw);

    return S_OK;
}

void CBattle_Pokemon::Priority_Update(_float fTimeDelta)
{
}

void CBattle_Pokemon::Update(_float fTimeDelta)
{
    m_PartObjects.for_each([&fTimeDelta](auto& Pair)
        {
            if (nullptr != Pair.second)
                Pair.second->Update(fTimeDelta);
        });
}

void CBattle_Pokemon::Late_Update(_float fTimeDelta)
{
    m_PartObjects.for_each([&fTimeDelta](auto& Pair)
        {
            if (nullptr != Pair.second)
                Pair.second->Late_Update(fTimeDelta);
        });
}

HRESULT CBattle_Pokemon::Render()
{
    return S_OK;
}

HRESULT CBattle_Pokemon::Ready_PartObjects(const POKEMON_DESC* pDesc)
{
    CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.strModelProtoTag = m_strSpeciesModelTag;
    BodyDesc.strShaderProtoTag = (0 != pDesc->strShaderProtoTag)
        ? pDesc->strShaderProtoTag
        : PROTO_COM_SHADER_POKEMON;
    BodyDesc.pRenderRule = m_pRenderRule;
    BodyDesc.iDefaultAnim = pDesc->iDefaultAnim;
    BodyDesc.bLoop = pDesc->bLoop;
    BodyDesc.fScale = pDesc->fScale;
    BodyDesc.bEnableRootMotion = false;
    BodyDesc.iRootMotionBoneIndex = 0;

    if (FAILED(__super::Add_PartObject(
        ETOUI(LEVEL::STATIC), m_strBodyProtoTag, PART_BODY, &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

CBattle_Pokemon * CBattle_Pokemon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBattle_Pokemon* pInstance = new CBattle_Pokemon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBattle_Pokemon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBattle_Pokemon::Clone(void* pArg)
{
    CBattle_Pokemon* pInstance = new CBattle_Pokemon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBattle_Pokemon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBattle_Pokemon::Free()
{
    __super::Free();
}