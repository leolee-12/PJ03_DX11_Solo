#include "Interaction_Encounter.h"

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

	// TODO: Capture 레벨 구현 후 옵션 B 로 확장
	// m_pGameInstance->Get_EncounterService()->Start(m_iSpeciesID, m_iLevel);
	// → CLevel_GamePlay::Request_Capture(CAPTURE_ENV{...}) 호출 흐름으로 연결 예정
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