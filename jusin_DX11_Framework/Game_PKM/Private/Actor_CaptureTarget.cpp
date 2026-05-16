#include "Actor_CaptureTarget.h"
#include "Body.h"
#include "Interaction_BallHit.h"

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

void CActor_CaptureTarget::Priority_Update(_float fTimeDelta) { __super::Priority_Update(fTimeDelta); }
void CActor_CaptureTarget::Update(_float fTimeDelta) { __super::Update(fTimeDelta); }
void CActor_CaptureTarget::Late_Update(_float fTimeDelta) { __super::Late_Update(fTimeDelta); }
HRESULT CActor_CaptureTarget::Render() { return __super::Render(); }

HRESULT CActor_CaptureTarget::Ready_Components(const ACTOR_CAPTURE_DESC* pDesc)
{
    CInteraction_BallHit::INTERACTION_BALLHIT_DESC HitDesc{};
    HitDesc.iBallItemID = pDesc->iInitialBallItemID;

    if (FAILED(__super::Add_Component(pDesc->iComponentLevel, PROTO_COM_INTERACTION_BALLHIT,
        COM_INTERACTION_BALLHIT, reinterpret_cast<CComponent**>(&m_pBallHit), &HitDesc)))
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

    __super::Free();
}