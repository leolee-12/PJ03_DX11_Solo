#include "Interaction_EventSequence.h"
#include "Event_Manager.h"
#include "Level_GamePlay.h"
#include "Actor_NPC.h"

#include "GameInstance.h"

CInteraction_EventSequence::CInteraction_EventSequence(ID3D11Device* pDevice, ID3D11DeviceContext*
	pContext)
	: CInteraction{ pDevice, pContext }
{
}

CInteraction_EventSequence::CInteraction_EventSequence(const CInteraction_EventSequence& Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CInteraction_EventSequence::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_EventSequence::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const INTERACTION_EVENT_SEQUENCE_DESC* pDesc =
		static_cast<const INTERACTION_EVENT_SEQUENCE_DESC*>(pArg);

	m_strSequenceID = pDesc->strSequenceID;
	m_eTrigger = pDesc->eTrigger;
	m_iPriority = pDesc->iPriority;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CInteraction_EventSequence::Supports(INTERACTION_EVENT eEvent) const
{
	return m_eTrigger == eEvent;
}

_bool CInteraction_EventSequence::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
	if (false == Supports(ctx.eEvent))
		return false;

	if (true == m_strSequenceID.empty())
		return false;

	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
		return false;

	if (true == pGamePlay->Is_Event_Playing())
		return false;

	if (true == pGamePlay->Is_Dialogue_Playing())
		return false;

	CEvent_Manager* pEventMgr = pGamePlay->Get_EventManager();
	if (nullptr == pEventMgr)
		return false;

	return nullptr != pEventMgr->Find_Sequence(m_strSequenceID);
}

void CInteraction_EventSequence::Execute(const INTERACTION_CONTEXT& ctx)
{
	if (false == CanInteract(ctx))
		return;

	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
		return;

	CEvent_Manager* pEventMgr = pGamePlay->Get_EventManager();
	if (nullptr == pEventMgr)
		return;

	CActor_NPC* pNPC = dynamic_cast<CActor_NPC*>(ctx.pTarget);
	if (nullptr != pNPC)
		pNPC->Face_To(XMLoadFloat4(&ctx.vCallerPosition));

	EVENT_CONTEXT tEventContext{};
	tEventContext.pGameInstance = m_pGameInstance;
	tEventContext.pLevelGamePlay = pGamePlay;
	tEventContext.pCaller = ctx.pCaller;
	tEventContext.pTarget = ctx.pTarget;
	tEventContext.eInteractionEvent = ctx.eEvent;

	if (FAILED(pEventMgr->Start_Sequence(m_strSequenceID, tEventContext)))
	{
#ifdef _DEBUG
		OutputDebugStringW(L"[Interaction_EventSequence] Start_Sequence failed\n");
#endif
	}
}

_int CInteraction_EventSequence::Get_Priority(const INTERACTION_CONTEXT& ctx) const
{
	return true == CanInteract(ctx) ? m_iPriority : 0;
}

CInteraction_EventSequence* CInteraction_EventSequence::Create(ID3D11Device* pDevice,
	ID3D11DeviceContext* pContext)
{
	CInteraction_EventSequence* pInstance = new CInteraction_EventSequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_EventSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CInteraction_EventSequence::Clone(void* pArg)
{
	CInteraction_EventSequence* pInstance = new CInteraction_EventSequence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_EventSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_EventSequence::Free()
{
	__super::Free();
}