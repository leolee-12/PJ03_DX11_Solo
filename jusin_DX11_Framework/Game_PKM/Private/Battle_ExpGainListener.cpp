#include "Battle_ExpGainListener.h"
#include "Battle_Manager.h"

CBattle_ExpGainListener::CBattle_ExpGainListener()
{
}

HRESULT CBattle_ExpGainListener::Initialize()
{
	return S_OK;
}

void CBattle_ExpGainListener::Bind(CBattle_Manager* pManager)
{
	m_pManager = pManager;
}

void CBattle_ExpGainListener::On_PokemonFainted(const EVENT_POKEMON_FAINTED& tEvent)
{
	if (nullptr == m_pManager)
		return;

	if (g_kBattleSide_Opponent != tEvent.iSide)
		return;

	if (m_bLockHeld)
		return;

	m_pManager->Add_Pacing_Lock();
	m_bLockHeld = true;
	m_fGraceTimer = 0.f;
}

void CBattle_ExpGainListener::Tick(_float fTimeDelta)
{
	if (false == m_bLockHeld || nullptr == m_pManager)
		return;

	m_fGraceTimer += fTimeDelta;

	if (m_fGraceTimer >= s_fGraceSeconds)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
		m_fGraceTimer = 0.f;
	}
}

CBattle_ExpGainListener* CBattle_ExpGainListener::Create()
{
	CBattle_ExpGainListener* pInstance = new CBattle_ExpGainListener();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBattle_ExpGainListener");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_ExpGainListener::Free()
{
	if (m_bLockHeld && nullptr != m_pManager)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}

	m_pManager = nullptr;

	__super::Free();
}