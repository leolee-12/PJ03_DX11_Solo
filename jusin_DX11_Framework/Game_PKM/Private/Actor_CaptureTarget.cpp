#include "Actor_CaptureTarget.h"
#include "Body.h"
#include "Interaction_BallHit.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

#include <cmath>

CActor_CaptureTarget::CActor_CaptureTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CActor{ pDevice, pContext }
{
    m_strName = L"CaptureTargetActor";
}

CActor_CaptureTarget::CActor_CaptureTarget(const CActor_CaptureTarget& Prototype)
    : CActor{ Prototype }
{
}

HRESULT CActor_CaptureTarget::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CActor_CaptureTarget::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    const ACTOR_CAPTURE_DESC* pDesc = static_cast<const ACTOR_CAPTURE_DESC*>(pArg);
    m_iSpeciesID = pDesc->iSpeciesID;
    m_iLevel = pDesc->iLevel;
    m_bCaughtBefore = pDesc->bCaughtBefore;

    if (nullptr != pDesc->pBodyDesc)
        m_strModelTag = pDesc->pBodyDesc->strModelProtoTag;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(pDesc->vSpawnPos.x, pDesc->vSpawnPos.y, pDesc->vSpawnPos.z, 1.f));

    m_vHomePos = pDesc->vSpawnPos;

    switch (m_iSpeciesID)
    {
    case 10: m_eMoveType = CAPTURE_MOVE_TYPE::STAY;     break;   // PM0010 - 정지
    case 43: m_eMoveType = CAPTURE_MOVE_TYPE::RUN_TURN; break;   // PM0043 - 좌우 달리기 + 턴
    case 41: m_eMoveType = CAPTURE_MOVE_TYPE::BAT;      break;   // PM0041 - 박쥐 패턴
    default: m_eMoveType = CAPTURE_MOVE_TYPE::STAY;     break;
    }

    /* RUN_TURN 의 달리기 속도(루트모션 델타 스케일). 다른 패턴은 미사용. */
    m_Tuning.fRootMotionScale = BattleAnim::Find_RootMotionScale(m_strModelTag);

    Cache_Members();
    Rebuild_InteractionCache();

    Play_IdleAnim();

    return S_OK;
}

void CActor_CaptureTarget::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CActor_CaptureTarget::Update(_float fTimeDelta)
{
    if (m_bAbsorbing)
    {
        m_fAbsorbElapsed += fTimeDelta;

        const _float fT = (m_fAbsorbDuration > 1e-6f)
            ? min(m_fAbsorbElapsed / m_fAbsorbDuration, 1.f)
            : 1.f;

        const _float fScale = m_bAbsorbReverse ? fT : (1.f - fT);

        const _vector vR = XMLoadFloat3(&m_vRightUnit) * fScale;
        const _vector vU = XMLoadFloat3(&m_vUpUnit) * fScale;
        const _vector vL = XMLoadFloat3(&m_vLookUnit) * fScale;

        m_pTransformCom->Set_State(STATE::RIGHT, vR);
        m_pTransformCom->Set_State(STATE::UP, vU);
        m_pTransformCom->Set_State(STATE::LOOK, vL);

        if (fT >= 1.f)
            m_bAbsorbing = false;
    }

    /* 이동은 Tick_Movement 에서. __super::Update(CActor) 가 파트 애님 갱신 후 호출하므로
       루트모션 델타가 준비된 시점이며, 아래 콜라이더 갱신이 갱신된 위치를 따라간다. */
    __super::Update(fTimeDelta);

    if (nullptr != m_pColliderCom)
        m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    if (m_fAnimDuration > 0.f)
    {
        m_fAnimTimer += fTimeDelta;
        if (m_fAnimTimer >= m_fAnimDuration)
            Play_IdleAnim();
    }
}

void CActor_CaptureTarget::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

#ifdef _DEBUG
    if (nullptr != m_pColliderCom)
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CActor_CaptureTarget::Render()
{
    return __super::Render();
}

_float3 CActor_CaptureTarget::Get_CaptureCenter() const
{
    const _vector vLocalCenter = XMLoadFloat3(&m_vCaptureCenter);

    const _vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
    const _vector vUp = m_pTransformCom->Get_State(STATE::UP);
    const _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

    const _vector vOffset =
        vRight * XMVectorGetX(vLocalCenter) +
        vUp * XMVectorGetY(vLocalCenter) +
        vLook * XMVectorGetZ(vLocalCenter);

    _float3 vCenter{};
    XMStoreFloat3(&vCenter, m_pTransformCom->Get_State(STATE::POSITION) + vOffset);
    return vCenter;
}

_float3 CActor_CaptureTarget::Get_EffectPivot() const
{
    return Get_CaptureCenter();
}

void CActor_CaptureTarget::Begin_Absorb()
{
    if (m_bAbsorbing)
        return;

    Cache_BasisIfNeeded();

    m_bAbsorbing = true;
    m_bAbsorbReverse = false;
    m_fAbsorbElapsed = 0.f;

    CEffect* pEffect = CEffect_Manager::GetInstance()->PlayAt(
        "ball_absorb",
        Get_EffectPivot());

    OutputDebugStringA(pEffect ? "[Absorb] effect ok\n" : "[Absorb] effect null\n");
}

void CActor_CaptureTarget::Begin_Appear()
{
    if (m_bAbsorbing)
        return;

    Cache_BasisIfNeeded();

    m_bAbsorbing = true;
    m_bAbsorbReverse = true;
    m_fAbsorbElapsed = 0.f;

    CEffect* pEffect = CEffect_Manager::GetInstance()->PlayAt(
        "ball_absorb",
        Get_EffectPivot());

    Play_AppearAnim();

    OutputDebugStringA(pEffect ? "[Appear] effect ok\n" : "[Appear] effect null\n");
}

void CActor_CaptureTarget::Play_IdleAnim()
{
    if (nullptr == m_pBody)
        return;

    m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strModelTag, ANIM_KIND::IDLE), true);
    m_eAnimKind = ANIM_KIND::IDLE;
    m_fAnimTimer = 0.f;
    m_fAnimDuration = 0.f;
}

void CActor_CaptureTarget::Play_AppearAnim()
{
    if (nullptr == m_pBody)
        return;

    m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strModelTag, ANIM_KIND::INTRO), false);
    m_eAnimKind = ANIM_KIND::INTRO;
    m_fAnimTimer = 0.f;
    m_fAnimDuration = 1.6f;
}

HRESULT CActor_CaptureTarget::Ready_Components(const ACTOR_CAPTURE_DESC* pDesc)
{
    CInteraction_BallHit::INTERACTION_BALLHIT_DESC HitDesc{};
    HitDesc.iBallItemID = pDesc->iInitialBallItemID;

    if (FAILED(__super::Add_Component(pDesc->iComponentLevel, PROTO_COM_INTERACTION_BALLHIT,
        COM_INTERACTION_BALLHIT, reinterpret_cast<CComponent**>(&m_pBallHit), &HitDesc)))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.vCenter = m_vCaptureCenter;
    SphereDesc.fRadius = m_fCaptureRadius;

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
        COM_COLLIDER_SPHERE, reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CActor_CaptureTarget::Ready_PartObjects(const ACTOR_CAPTURE_DESC* pDesc)
{
    if (nullptr == pDesc->pBodyDesc)
        return E_FAIL;

    pDesc->pBodyDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(pDesc->iBodyProtoLevel, pDesc->strBodyProtoTag,
        PART_BODY, pDesc->pBodyDesc)))
        return E_FAIL;

    return S_OK;
}

void CActor_CaptureTarget::Cache_Members()
{
    m_pBody = Get_Part<CBody>(PART_BODY);
}

void CActor_CaptureTarget::Cache_BasisIfNeeded()
{
    if (m_bBasisCached)
        return;

    XMStoreFloat3(&m_vRightUnit, XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)));
    XMStoreFloat3(&m_vUpUnit, XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP)));
    XMStoreFloat3(&m_vLookUnit, XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)));

    m_bBasisCached = true;
}

void CActor_CaptureTarget::Reset_Move()
{
    m_fMoveElapsed = 0.f;
    m_iRunDir = 1;
    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(m_vHomePos.x, m_vHomePos.y, m_vHomePos.z, 1.f));
}

void CActor_CaptureTarget::Tick_Movement(_float fTimeDelta)
{
    if (m_bAbsorbing || false == m_bMoveActive)
        return;

    switch (m_eMoveType)
    {
    case CAPTURE_MOVE_TYPE::BAT:
    {
        /* 위치만 갱신(회전/스케일은 LookAt·absorb 가 관리). 콜라이더는 Update 의 갱신이 따라감. */
        m_fMoveElapsed += fTimeDelta;
        const _float3 vOff = Compute_MoveOffset(m_fMoveElapsed);
        m_pTransformCom->Set_State(STATE::POSITION,
            XMVectorSet(m_vHomePos.x + vOff.x, m_vHomePos.y + vOff.y, m_vHomePos.z + vOff.z, 1.f));
        break;
    }
    case CAPTURE_MOVE_TYPE::RUN_TURN:
        Tick_RunTurn(fTimeDelta);
        break;

    default:   // STAY
        break;
    }
}

void CActor_CaptureTarget::Tick_RunTurn(_float fTimeDelta)
{
    if (nullptr == m_pBody)
        return;

    constexpr _float RUN_HALF_RANGE = 1.2f;   // 좌우 끝점(홈 기준)

    const _float fOffX =
        XMVectorGetX(m_pTransformCom->Get_State(STATE::POSITION)) - m_vHomePos.x;

    /* 끝점 도달 시 방향 반전. 다음 Tick_RootMotionMovement 에서 180° 차이 ->
       피벗(제자리 IDLE 회전) 후 반대로 달림. */
    if (m_iRunDir > 0 && fOffX >= RUN_HALF_RANGE)       m_iRunDir = -1;
    else if (m_iRunDir < 0 && fOffX <= -RUN_HALF_RANGE) m_iRunDir = 1;

    /* 정확한 ±X 면 끝점 반전 시 Face_Direction 의 lerp 가 0을 통과하며 제자리 정지(180° degeneracy).
       살짝 -Z(카메라 쪽) 바이어스를 줘 회피 → 턴이 카메라를 향하며 자연스럽게 돌아간다.
       위치는 아래 z 클램프로 순수 좌우 유지. */
    constexpr _float TURN_BIAS_Z = -0.2f;
    const _vector vMoveDir = (m_iRunDir > 0)
        ? XMVector3Normalize(XMVectorSet(1.f, 0.f, TURN_BIAS_Z, 0.f))
        : XMVector3Normalize(XMVectorSet(-1.f, 0.f, TURN_BIAS_Z, 0.f));

    /* 피벗(턴) 중이면 IDLE, 달리는 중이면 RUN. (Pivoting 은 직전 프레임 결과 - 1프레임 지연 무시 가능) */
    const ANIM_KIND eKind = m_MoveState.Pivoting ? ANIM_KIND::IDLE : ANIM_KIND::RUN;
    m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strModelTag, eKind), true);

    Tick_RootMotionMovement(vMoveDir, true, m_pBody->Get_RootMotionDelta(), nullptr, fTimeDelta);

    /* 좌우(X) 직선 유지 - y/z 드리프트 제거(평면 스테이지). */
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    vPos = XMVectorSetY(vPos, m_vHomePos.y);
    vPos = XMVectorSetZ(vPos, m_vHomePos.z);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(vPos, 1.f));
}

_float3 CActor_CaptureTarget::Compute_MoveOffset(_float fElapsed) const
{
    if (CAPTURE_MOVE_TYPE::BAT != m_eMoveType)
        return _float3(0.f, 0.f, 0.f);

    /* 중앙 -> 좌상 -> 좌하 -> 우상 -> 우하 반복(빠르게). 각 도착점에서 약간 대기. 공중 호버. */
    constexpr _float Tmove = 0.16f;   // 구간 이동 시간(짧을수록 빠름)
    constexpr _float Tdwell = 0.22f;  // 도착점 대기 시간
    constexpr _float A = 1.45f;       // 좌우(대각 도달점 더 멀게)
    constexpr _float TOP = 1.75f;     // 위
    constexpr _float BOT = 0.1f;      // 아래(바닥 위 유지)
    constexpr _float MID = 0.85f;     // 중앙 높이(공중)

    const _float3 W[5] =
    {
        _float3(0.f, MID, 0.f),    // 중앙
        _float3(-A,  TOP, 0.f),    // 좌상
        _float3(-A,  BOT, 0.f),    // 좌하
        _float3( A,  TOP, 0.f),    // 우상
        _float3( A,  BOT, 0.f),    // 우하
    };

    const _float fBeat = Tmove + Tdwell;
    const _int   n = static_cast<_int>(floorf(fElapsed / fBeat));
    const _float fPhase = fElapsed - static_cast<_float>(n) * fBeat;

    const _int   i = ((n % 5) + 5) % 5;       // 현재 도착점에서 출발
    const _int   j = (i + 1) % 5;             // 다음 도착점

    /* 이동 구간이면 보간, 대기 구간이면 목적지(W[j])에 정지. */
    const _float u = (fPhase < Tmove) ? (fPhase / Tmove) : 1.f;

    return _float3(
        W[i].x + (W[j].x - W[i].x) * u,
        W[i].y + (W[j].y - W[i].y) * u,
        0.f);
}

CActor_CaptureTarget* CActor_CaptureTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CActor_CaptureTarget* pInstance = new CActor_CaptureTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CActor_CaptureTarget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CActor_CaptureTarget::Clone(void* pArg)
{
    CActor_CaptureTarget* pInstance = new CActor_CaptureTarget(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CActor_CaptureTarget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CActor_CaptureTarget::Free()
{
    Safe_Release(m_pBallHit);
    Safe_Release(m_pColliderCom);

    __super::Free();
}