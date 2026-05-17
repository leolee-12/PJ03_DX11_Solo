#include "Actor_CaptureTarget.h"
#include "Body.h"
#include "Interaction_BallHit.h"


#include "Collider.h"
#include "Bounding_Sphere.h"
#include "GameInstance.h"

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
