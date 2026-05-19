#include "Actor_NPC.h"
#include "Body.h"
#include "Interaction_Dialogue.h"

CActor_NPC::CActor_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
	m_strName = L"NPCActor";
}

CActor_NPC::CActor_NPC(const CActor_NPC& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CActor_NPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor_NPC::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const ACTOR_NPC_DESC* pDesc = static_cast<const ACTOR_NPC_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(pDesc->vSpawnPos.x, pDesc->vSpawnPos.y, pDesc->vSpawnPos.z, 1.f));

	m_iSpawnRectID = pDesc->iSpawnRectID;
	if (pDesc->bIsTrainer || pDesc->bApplyInitialRotation)
	{
		m_pTransformCom->Rotation(0.f, pDesc->fInitialRotationY, 0.f);
	}

	Cache_Members();
	Rebuild_InteractionCache();

	return S_OK;
}

void CActor_NPC::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CActor_NPC::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Update_FaceTurn(fTimeDelta);
}

void CActor_NPC::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CActor_NPC::Render()
{
	return __super::Render();
}

void XM_CALLCONV CActor_NPC::Face_To(_fvector vTargetPos)
{
	XMStoreFloat3(&m_vFaceTurnTarget, vTargetPos);
	m_bFaceTurnActive = true;
}

HRESULT CActor_NPC::Ready_Components(const ACTOR_NPC_DESC* pDesc)
{
	CInteraction_Dialogue::INTERACTION_DIALOGUE_DESC DialogueDesc;
	DialogueDesc.strDialogueKey = pDesc->strDialogueKey;

	if (FAILED(__super::Add_Component(pDesc->iComponentLevel, PROTO_COM_INTERACTION_DIALOGUE,
		COM_INTERACTION_DIALOGUE, reinterpret_cast<CComponent**>(&m_pDialogue), &DialogueDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_NPC::Ready_PartObjects(const ACTOR_NPC_DESC* pDesc)
{
	if (nullptr == pDesc->pBodyDesc)
		return E_FAIL;

	pDesc->pBodyDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PartObject(pDesc->iBodyProtoLevel, pDesc->strBodyProtoTag,
		PART_BODY, pDesc->pBodyDesc)))
		return E_FAIL;

	return S_OK;
}

void CActor_NPC::Cache_Members()
{
	m_pBody = Get_Part<CBody>(PART_BODY);
}

void CActor_NPC::Update_FaceTurn(_float fTimeDelta)
{
	if (false == m_bFaceTurnActive)
		return;

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTarget = XMLoadFloat3(&m_vFaceTurnTarget);
	vTarget = XMVectorSetY(vTarget, XMVectorGetY(vPos));

	_vector vTargetDir = vTarget - vPos;
	vTargetDir = XMVectorSetY(vTargetDir, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vTargetDir)) <= 0.0001f)
	{
		m_bFaceTurnActive = false;
		return;
	}

	vTargetDir = XMVector3Normalize(vTargetDir);

	_vector vCurLook = m_pTransformCom->Get_State(STATE::LOOK);
	vCurLook = XMVectorSetY(vCurLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vCurLook)) <= 0.0001f)
	{
		m_bFaceTurnActive = false;
		return;
	}

	vCurLook = XMVector3Normalize(vCurLook);

	const _float fCurYaw = atan2f(XMVectorGetX(vCurLook), XMVectorGetZ(vCurLook));
	const _float fTargetYaw = atan2f(XMVectorGetX(vTargetDir), XMVectorGetZ(vTargetDir));

	_float fDeltaYaw = fTargetYaw - fCurYaw;

	while (fDeltaYaw > XM_PI)
		fDeltaYaw -= XM_2PI;
	while (fDeltaYaw < -XM_PI)
		fDeltaYaw += XM_2PI;

	const _float fStep = m_fFaceTurnRadiansPerSec * fTimeDelta;

	if (fabsf(fDeltaYaw) <= fStep || fabsf(fDeltaYaw) <= XMConvertToRadians(1.f))
	{
		m_pTransformCom->Rotation(0.f, fTargetYaw, 0.f);
		m_bFaceTurnActive = false;
		return;
	}

	const _float fNextYaw = fCurYaw + (fDeltaYaw > 0.f ? fStep : -fStep);
	m_pTransformCom->Rotation(0.f, fNextYaw, 0.f);
}

CActor_NPC* CActor_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_NPC* pInstance = new CActor_NPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CActor_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_NPC::Clone(void* pArg)
{
	CActor_NPC* pInstance = new CActor_NPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CActor_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_NPC::Free()
{
	Safe_Release(m_pDialogue);

	__super::Free();
}