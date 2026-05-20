#include "Actor_CaptureTarget.h"
#include "Body.h"
#include "Interaction_BallHit.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

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

    __super::Update(fTimeDelta);

    if (nullptr != m_pColliderCom)
        m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
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

void CActor_CaptureTarget::Begin_Absorb()
{
    if (m_bAbsorbing)
        return;

    Cache_BasisIfNeeded();

    m_bAbsorbing = true;
    m_bAbsorbReverse = false;
    m_fAbsorbElapsed = 0.f;

    CEffect::EFFECT_DESC::ATTACH_INFO tAttach{};
    tAttach.eKind = CEffect::EFFECT_DESC::ATTACH_INFO::KIND::NONE;

    m_pAbsorbEffect = CEffect_Manager::GetInstance()->Spawn(
        "ball_absorb",
        Get_CaptureCenter(),
        ETOUI(LEVEL::CAPTURE),
        LAYER_EFFECT,
        tAttach);

    OutputDebugStringA(m_pAbsorbEffect ? "[Absorb] effect ok\n" : "[Absorb] effect null\n");
}

void CActor_CaptureTarget::Begin_Appear()
{
    if (m_bAbsorbing)
        return;

    Cache_BasisIfNeeded();

    m_bAbsorbing = true;
    m_bAbsorbReverse = true;
    m_fAbsorbElapsed = 0.f;

    CEffect::EFFECT_DESC::ATTACH_INFO tAttach{};
    tAttach.eKind = CEffect::EFFECT_DESC::ATTACH_INFO::KIND::NONE;

    m_pAbsorbEffect = CEffect_Manager::GetInstance()->Spawn(
        "ball_absorb",
        Get_CaptureCenter(),
        ETOUI(LEVEL::CAPTURE),
        LAYER_EFFECT,
        tAttach);

    OutputDebugStringA(m_pAbsorbEffect ? "[Appear] effect ok\n" : "[Appear] effect null\n");
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
    m_pAbsorbEffect = nullptr;

    Safe_Release(m_pBallHit);
    Safe_Release(m_pColliderCom);

    __super::Free();
}