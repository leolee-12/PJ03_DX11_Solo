#include "Interaction_Encounter.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Level.h"

CInteraction_Encounter::CInteraction_Encounter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteraction{ pDevice, pContext }
{
}

CInteraction_Encounter::CInteraction_Encounter(const CInteraction_Encounter& Prototype)
	: CInteraction{ Prototype }
{
}

HRESULT CInteraction_Encounter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Encounter::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const INTERACTION_ENCOUNTER_DESC* pDesc = static_cast<const INTERACTION_ENCOUNTER_DESC*>(pArg);
	m_iSpeciesID = pDesc->iSpeciesID;
	m_iLevel = pDesc->iLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CInteraction_Encounter::Supports(INTERACTION_EVENT eEvent) const
{
	return INTERACTION_EVENT::TOUCH == eEvent;
}

_bool CInteraction_Encounter::CanInteract(const INTERACTION_CONTEXT& ctx) const
{
	if (!Supports(ctx.eEvent))
		return false;

	// TODO: 어댑터 도입 후
	// if (m_pGameInstance->Get_SceneService()->Is_ChangingScene())
	//     return false;
	// if (m_pGameInstance->Get_EncounterService()->Is_Locked())
	//     return false;

	return true;
}

void CInteraction_Encounter::Execute(const INTERACTION_CONTEXT& ctx)
{
	wchar_t szLog[128] = {};
	swprintf_s(szLog, L"[Interaction_Encounter] Execute: SpeciesID=%u, Level=%u\n",
		m_iSpeciesID, m_iLevel);
	OutputDebugStringW(szLog);

	/* 현재 활성 레벨이 GAMEPLAY 일 때만 Capture 진입 요청.
	   GAMEPLAY 외 상태에서 Wild 액터에 닿는 경로는 정상 동선이 아니므로 무시. */
	CLevel* pCurrent = m_pGameInstance->Get_CurrentLevelPtr();
	CLevel_GamePlay* pGamePlay = dynamic_cast<CLevel_GamePlay*>(pCurrent);
	if (nullptr == pGamePlay)
	{
		OutputDebugStringW(L"[Interaction_Encounter] Skip: current level is not GAMEPLAY\n");
		return;
	}

	CAPTURE_ENV tEnv = {};
	tEnv.iSpeciesID = m_iSpeciesID;
	tEnv.iLevel = m_iLevel;
	tEnv.iInitialBallItemID = 0;   // 후속 단위에서 야생 액터 기본 볼 ID 로 채움
	tEnv.iZoneID = 0;   // 후속 단위에서 GAMEPLAY 의 현재 존 ID 주입

	pGamePlay->Request_Capture(tEnv);
}

CInteraction_Encounter* CInteraction_Encounter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInteraction_Encounter* pInstance = new CInteraction_Encounter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_Encounter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CInteraction_Encounter::Clone(void* pArg)
{
	CInteraction_Encounter* pInstance = new CInteraction_Encounter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_Encounter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_Encounter::Free()
{
	__super::Free();
}