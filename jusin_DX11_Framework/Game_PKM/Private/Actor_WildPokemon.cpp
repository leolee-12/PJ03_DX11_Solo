#include "Actor_WildPokemon.h"
#include "Body.h"
#include "Interaction_Encounter.h"

#include "GameInstance.h"
//#include "Collider.h"

CActor_WildPokemon::CActor_WildPokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CActor{ pDevice, pContext }
{
    m_strName = L"WildPokemonActor";
}

CActor_WildPokemon::CActor_WildPokemon(const CActor_WildPokemon& Prototype)
    : CActor{ Prototype }
{
}

HRESULT CActor_WildPokemon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CActor_WildPokemon::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    const ACTOR_WILD_DESC* pDesc = static_cast<const ACTOR_WILD_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(pDesc->vSpawnPos.x, pDesc->vSpawnPos.y, pDesc->vSpawnPos.z, 1.f));

    Cache_Members();
    Rebuild_InteractionCache();

    return S_OK;
}

void CActor_WildPokemon::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CActor_WildPokemon::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (nullptr != m_pColliderCom)
        m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CActor_WildPokemon::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

#ifdef _DEBUG
    if (nullptr != m_pColliderCom)
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CActor_WildPokemon::Render()
{
    return __super::Render();
}

HRESULT CActor_WildPokemon::Ready_Components(const ACTOR_WILD_DESC* pDesc)
{
    CInteraction_Encounter::INTERACTION_ENCOUNTER_DESC EncDesc{};
    EncDesc.iSpeciesID = pDesc->iSpeciesID;
    EncDesc.iLevel = pDesc->iLevel;

    if (FAILED(__super::Add_Component(pDesc->iComponentLevel, PROTO_COM_INTERACTION_ENCOUNTER,
        COM_INTERACTION_ENCOUNTER, reinterpret_cast<CComponent**>(&m_pEncounter), &EncDesc)))
        return E_FAIL;

    // SPHERE Collider — TOUCH 트리거용
    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);
    SphereDesc.fRadius = 0.6f;

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
        COM_COLLIDER_SPHERE, reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CActor_WildPokemon::Ready_PartObjects(const ACTOR_WILD_DESC* pDesc)
{
    if (nullptr == pDesc->pBodyDesc)
        return E_FAIL;

    pDesc->pBodyDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(pDesc->iBodyProtoLevel, pDesc->strBodyProtoTag,
        PART_BODY, pDesc->pBodyDesc)))
        return E_FAIL;

    return S_OK;
}

void CActor_WildPokemon::Cache_Members()
{
    m_pBody = Get_Part<CBody>(PART_BODY);
}

CActor_WildPokemon* CActor_WildPokemon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CActor_WildPokemon* pInstance = new CActor_WildPokemon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CActor_WildPokemon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CActor_WildPokemon::Clone(void* pArg)
{
    CActor_WildPokemon* pInstance = new CActor_WildPokemon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CActor_WildPokemon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CActor_WildPokemon::Free()
{
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pEncounter);

    __super::Free();
}