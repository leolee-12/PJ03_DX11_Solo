#include "Interaction_Dialogue.h"
#include "Level_GamePlay.h"
#include "Actor_NPC.h"

#include "GameInstance.h"

CInteraction_Dialogue::CInteraction_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteraction{ pDevice, pContext }
{
}

CInteraction_Dialogue::CInteraction_Dialogue(const CInteraction_Dialogue& Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CInteraction_Dialogue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Dialogue::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const INTERACTION_DIALOGUE_DESC* pDesc = static_cast<const INTERACTION_DIALOGUE_DESC*>(pArg);
	m_strDialogueKey = pDesc->strDialogueKey;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CInteraction_Dialogue::Supports(INTERACTION_EVENT eEvent) const
{
	return INTERACTION_EVENT::TALK == eEvent;
}

_bool CInteraction_Dialogue::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
	if (false == Supports(ctx.eEvent))
		return false;

	if (true == m_strDialogueKey.empty())
		return false;

	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
		return false;

	if (true == pGamePlay->Is_Dialogue_Playing())
		return false;

	return true;
}

void CInteraction_Dialogue::Execute(const INTERACTION_CONTEXT& ctx)
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

	if (false == pGamePlay->Start_Dialogue(m_strDialogueKey))
	{
		OutputDebugStringW(L"[Interaction_Dialogue] Start_Dialogue failed\n");
		return;
	}
}

CInteraction_Dialogue* CInteraction_Dialogue::Create(ID3D11Device* pDevice, ID3D11DeviceContext*
	pContext)
{
	CInteraction_Dialogue* pInstance = new CInteraction_Dialogue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CInteraction_Dialogue::Clone(void* pArg)
{
	CInteraction_Dialogue* pInstance = new CInteraction_Dialogue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_Dialogue::Free()
{
	__super::Free();
}