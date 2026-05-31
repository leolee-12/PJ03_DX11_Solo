#include "Interaction_DialogueBattle.h"
#include "Level_GamePlay.h"
#include "Actor_NPC.h"

#include "GameInstance.h"

CInteraction_DialogueBattle::CInteraction_DialogueBattle(ID3D11Device* pDevice, ID3D11DeviceContext*
	pContext)
	: CInteraction{ pDevice, pContext }
{
}

CInteraction_DialogueBattle::CInteraction_DialogueBattle(const CInteraction_DialogueBattle&
	Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CInteraction_DialogueBattle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_DialogueBattle::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const INTERACTION_DIALOGUE_BATTLE_DESC* pDesc = static_cast<const
		INTERACTION_DIALOGUE_BATTLE_DESC*>(pArg);

	m_strDialogueKey = pDesc->strDialogueKey;
	m_iTrainerID = pDesc->iTrainerID;
	m_bOneShot = pDesc->bOneShot;

	m_tBattleEnv.eEnvironment = pDesc->eEnvironment;
	m_tBattleEnv.eRule = pDesc->eRule;
	m_tBattleEnv.iOpponentTrainerID = pDesc->iTrainerID;
	m_tBattleEnv.iBGResourceID = pDesc->iBGResourceID;
	m_tBattleEnv.iZoneID = pDesc->iZoneID;
	m_tBattleEnv.bCanRun = pDesc->bCanRun;
	m_tBattleEnv.bCanCapture = pDesc->bCanCapture;
	m_tBattleEnv.bExpGain = pDesc->bExpGain;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CInteraction_DialogueBattle::Supports(INTERACTION_EVENT eEvent) const
{
	return INTERACTION_EVENT::TALK == eEvent;
}

_bool CInteraction_DialogueBattle::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
	if (false == Supports(ctx.eEvent))
		return false;

	if (0 == m_iTrainerID)
		return false;

	if (true == m_bOneShot && true == m_bAlreadyBattled)
		return false;

	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
		return false;

	if (true == pGamePlay->Is_Dialogue_Playing())
		return false;

	if (RUN_STATE::IDLE != m_eRunState)
		return false;

	return true;
}

void CInteraction_DialogueBattle::Execute(const INTERACTION_CONTEXT& ctx)
{
	if (false == CanInteract(ctx))
		return;

	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
		return;

	CActor_NPC* pNPC = dynamic_cast<CActor_NPC*>(ctx.pTarget);
	if (nullptr != pNPC)
		pNPC->Face_To(XMLoadFloat4(&ctx.vCallerPosition));

	_bool bStarted = false;

	if (true == m_strDialogueKey.empty())
		bStarted = pGamePlay->Start_Dialogue_Text(L"승부다!");
	else
		bStarted = pGamePlay->Start_Dialogue(m_strDialogueKey);

	if (false == bStarted)
	{
		OutputDebugStringW(L"[Interaction_DialogueBattle] Start_Dialogue failed\n");
		return;
	}

	m_eRunState = RUN_STATE::DIALOGUE;
}

_int CInteraction_DialogueBattle::Get_Priority(const INTERACTION_CONTEXT& ctx) const
{
	return 100;
}

void CInteraction_DialogueBattle::Tick(_float fTimeDelta)
{
	if (RUN_STATE::DIALOGUE != m_eRunState)
		return;

	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
	{
		m_eRunState = RUN_STATE::IDLE;
		return;
	}

	if (true == pGamePlay->Is_Dialogue_Playing())
		return;

	const _bool bOk = pGamePlay->Request_Battle(m_tBattleEnv);

	if (true == bOk && true == m_bOneShot)
		m_bAlreadyBattled = true;

	m_eRunState = RUN_STATE::DONE;

	OutputDebugStringW(bOk
		? L"[Interaction_DialogueBattle] Request_Battle = true\n"
		: L"[Interaction_DialogueBattle] Request_Battle = false\n");
}

CInteraction_DialogueBattle* CInteraction_DialogueBattle::Create(ID3D11Device* pDevice,
	ID3D11DeviceContext* pContext)
{
	CInteraction_DialogueBattle* pInstance = new CInteraction_DialogueBattle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_DialogueBattle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CInteraction_DialogueBattle::Clone(void* pArg)
{
	CInteraction_DialogueBattle* pInstance = new CInteraction_DialogueBattle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_DialogueBattle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_DialogueBattle::Free()
{
	__super::Free();
}