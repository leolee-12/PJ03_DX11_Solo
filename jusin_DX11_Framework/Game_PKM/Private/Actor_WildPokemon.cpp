#include "Actor_WildPokemon.h"
#include "Body.h"
#include "Interaction_Encounter.h"

#include "GameInstance.h"

namespace
{
    constexpr _float kIdleMinSec = 1.5f;
    constexpr _float kIdleMaxSec = 4.0f;
    constexpr _float kArriveEpsilon = 0.15f;
    constexpr _float kRectTooSmallArea = 1.0f;
    constexpr _float kMoveTimeoutSec = 5.0f;   // 루트모션 delta=0 / 막힘 케이스 회피
}

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

    // S2 추가 — desc 의 SpawnRect 페이로드 캐싱 (사용은 S3 배회 로직)
    m_iSpawnRectID = pDesc->iSpawnRectID;
    m_vSpawnAnchor = pDesc->vSpawnAnchor;
    m_fLeashRadius = pDesc->fLeashRadius;
    m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
    m_tSpawnRectDesc = pDesc->tSpawnRectDesc;

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

    Tick_Wander(fTimeDelta);

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

    // S3 추가 — Navigation 컴포넌트
    // INVALID_NAV_CELL 을 _int 로 cast 하면 -1 이 되어 CNavigation::Set_CurrentCellIndex 가 invalid 처리.
    CNavigation::NAVIGATION_DESC NaviDesc{ static_cast<_int>(pDesc->iCurrentCellIndex) };

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_NAVIGATION_MAP,
        COM_NAVIGATION, reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
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

void CActor_WildPokemon::Tick_Wander(_float fTimeDelta)
{
    if (nullptr == m_pNavigationCom || nullptr == m_pBody) return;

    _vector vMoveDir = XMVectorZero();
    _bool   bHasInput = false;

    switch (m_eWanderState)
    {
    case WANDER_STATE::IDLE:
    {
        m_fWanderTimer -= fTimeDelta;
        m_fMoveDuration = 0.f;

        if (m_fWanderTimer <= 0.f)
        {
            if (Choose_WanderTarget())
                m_eWanderState = WANDER_STATE::MOVING;
            else
                m_fWanderTimer = SpawnMath::RandomFloat(kIdleMinSec, kIdleMaxSec);
        }
        // IDLE: vMoveDir = 0, bHasInput = false → Tick_RootMotionMovement 가 정지 분기로 빠짐.
        break;
    }

    case WANDER_STATE::MOVING:
    {
        const _vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);
        const _vector vTargetPos = XMVectorSetW(XMLoadFloat3(&m_vMoveTarget), 1.f);

        // XZ 평면 거리로 도착 판정 (Y 는 NavMesh 투영으로 자동 보정되므로 비교 제외)
        const _vector vDeltaXZ = XMVectorSet(
            XMVectorGetX(vTargetPos) - XMVectorGetX(vCurPos),
            0.f,
            XMVectorGetZ(vTargetPos) - XMVectorGetZ(vCurPos),
            0.f);
        const _float fDistSq = XMVectorGetX(XMVector3LengthSq(vDeltaXZ));

        if (fDistSq < kArriveEpsilon * kArriveEpsilon)
        {
            m_eWanderState = WANDER_STATE::IDLE;
            m_fWanderTimer = SpawnMath::RandomFloat(kIdleMinSec, kIdleMaxSec);
            m_fMoveDuration = 0.f;
            break;
        }

        // 루트모션 delta=0 / 막힘 케이스 — 일정 시간 안에 도착 못 하면 새 타깃 시도
        m_fMoveDuration += fTimeDelta;
        if (m_fMoveDuration > kMoveTimeoutSec)
        {
            m_eWanderState = WANDER_STATE::IDLE;
            m_fWanderTimer = SpawnMath::RandomFloat(kIdleMinSec, kIdleMaxSec);
            m_fMoveDuration = 0.f;
            break;
        }

        // 의도 방향 — Face_Direction 가 XZ 만 사용. 정규화는 Tick_RootMotionMovement 내부에서 수행됨.
        vMoveDir = vDeltaXZ;
        bHasInput = true;
        break;
    }

    default: break;
    }

    // Player_LGPE 와 동일 — 회전 + 루트모션 delta 기반 위치 갱신.
    Tick_RootMotionMovement(vMoveDir, bHasInput,
        m_pBody->Get_RootMotionDelta(), m_pNavigationCom, fTimeDelta);
}

_bool CActor_WildPokemon::Choose_WanderTarget()
{
    if (nullptr == m_pNavigationCom) return false;

    const _int iCurrentCell = m_pNavigationCom->Get_CurrentCellIndex();
    if (iCurrentCell < 0) return false;
    const _uint iCurrentCellU = static_cast<_uint>(iCurrentCell);

    for (_uint i = 0; i < g_kMaxWanderAttempts; ++i)
    {
        _float3 vCandidate{};

        if (m_bUseRectWander)
        {
            vCandidate = SpawnMath::Make_RandomPointInRect(m_tSpawnRectDesc);
        }
        else
        {
            const _float fAngle = SpawnMath::RandomFloat(0.f, XM_2PI);
            const _float fDist = SpawnMath::RandomFloat(0.f, m_fLeashRadius);
            vCandidate.x = m_vSpawnAnchor.x + cosf(fAngle) * fDist;
            vCandidate.y = m_vSpawnAnchor.y;
            vCandidate.z = m_vSpawnAnchor.z + sinf(fAngle) * fDist;
        }

        _float3 vNavPos = {};
        _uint   iCellIndex = INVALID_NAV_CELL;
        if (!m_pNavigationCom->Project_PointToNavigation(
            vCandidate, m_tSpawnRectDesc.fProjectRadius,
            m_tSpawnRectDesc.iAllowedAreaMask,
            &vNavPos, &iCellIndex))
            continue;

        if (m_bUseRectWander)
        {
            if (!SpawnMath::Is_PointInsideRectXZ(vNavPos, m_tSpawnRectDesc))
                continue;
        }

        if (!m_pNavigationCom->Is_Reachable(
            iCurrentCellU, iCellIndex, m_tSpawnRectDesc.iAllowedAreaMask))
            continue;

        m_vMoveTarget = vNavPos;
        m_iTargetCellIndex = iCellIndex;
        return true;
    }
    return false;
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
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pEncounter);

    __super::Free();
}