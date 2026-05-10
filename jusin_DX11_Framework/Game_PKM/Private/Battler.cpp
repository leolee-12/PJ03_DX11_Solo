#include "Battler.h"

CBattler::CBattler()
{
}

HRESULT CBattler::Initialize(const BATTLER_DESC& tDesc)
{
	if (nullptr == tDesc.pInstance)
		return E_FAIL;

	if (tDesc.iSide >= g_kBattleSideCount)
		return E_FAIL;

	if (tDesc.iSlotIndex >= g_kMaxSlotsPerSide)
		return E_FAIL;

	m_iSide = tDesc.iSide;
	m_iSlotIndex = tDesc.iSlotIndex;
	m_pInstance = tDesc.pInstance;

	Reset_BattleSlot(m_tSlot);
	m_tSlot.pPokemon = m_pInstance;

	return S_OK;
}

_ushort CBattler::Get_CurrentHP() const
{
	return (nullptr != m_pInstance) ? m_pInstance->iCurrentHP : 0;
}

_ushort CBattler::Get_MaxHP() const
{
	return (nullptr != m_pInstance) ? m_pInstance->iStat[static_cast<size_t>(STAT::HP)] : 0;
}

STATUS_CONDITION CBattler::Get_Status() const
{
	return (nullptr != m_pInstance) ? m_pInstance->eStatus : STATUS_CONDITION::NONE;
}

_uint CBattler::Get_MoveID(_uint iIndex) const
{
	if (nullptr == m_pInstance || iIndex >= g_kMaxMovesPerPokemon)
		return 0;

	return m_pInstance->iMoves[iIndex];
}

_ubyte CBattler::Get_PP(_uint iIndex) const
{
	if (nullptr == m_pInstance || iIndex >= g_kMaxMovesPerPokemon)
		return 0;

	return m_pInstance->iCurrentPP[iIndex];
}

_ushort CBattler::Get_Stat(STAT eStat) const
{
	const size_t iIndex = static_cast<size_t>(eStat);

	if (nullptr == m_pInstance || iIndex >= static_cast<size_t>(STAT::END))
		return 0;

	return m_pInstance->iStat[iIndex];
}

_byte CBattler::Get_StatStage(STAGE_INDEX eIndex) const
{
	const size_t iIndex = static_cast<size_t>(eIndex);

	if (iIndex >= static_cast<size_t>(STAGE_INDEX::COUNT))
		return 0;

	return m_tSlot.iStatStage[iIndex];
}

_ushort CBattler::Apply_Damage(_ushort iAmount)
{
	if (nullptr == m_pInstance)
		return 0;

	const _ushort iApplied = (iAmount > m_pInstance->iCurrentHP) ? m_pInstance->iCurrentHP : iAmount;
	m_pInstance->iCurrentHP -= iApplied;

	return iApplied;
}

_ushort CBattler::Apply_Heal(_ushort iAmount)
{
	if (nullptr == m_pInstance)
		return 0;

	const _ushort iMax = Get_MaxHP();
	const _ushort iRoom = (iMax > m_pInstance->iCurrentHP) ? static_cast<_ushort>(iMax -
		m_pInstance->iCurrentHP) : 0;
	const _ushort iApplied = (iAmount > iRoom) ? iRoom : iAmount;

	m_pInstance->iCurrentHP += iApplied;

	return iApplied;
}

_bool CBattler::Set_Status(STATUS_CONDITION eStatus)
{
	if (nullptr == m_pInstance)
		return false;

	if (STATUS_CONDITION::NONE != m_pInstance->eStatus)
		return false;

	m_pInstance->eStatus = eStatus;
	return true;
}

void CBattler::Clear_Status()
{
	if (nullptr != m_pInstance)
		m_pInstance->eStatus = STATUS_CONDITION::NONE;
}

_byte CBattler::Modify_StatStage(STAGE_INDEX eIndex, _byte iDelta)
{
	const size_t iIndex = static_cast<size_t>(eIndex);

	if (iIndex >= static_cast<size_t>(STAGE_INDEX::COUNT))
		return 0;

	_byte iNew = static_cast<_byte>(m_tSlot.iStatStage[iIndex] + iDelta);

	if (iNew > 6)
		iNew = 6;

	if (iNew < -6)
		iNew = -6;

	m_tSlot.iStatStage[iIndex] = iNew;

	return iNew;
}

void CBattler::Set_Volatile_Flag(_uint iFlag, _ubyte iTurns)
{
	m_tSlot.iVolatileFlags |= iFlag;
	(void)iTurns;
}

void CBattler::Clear_Volatile_Flag(_uint iFlag)
{
	m_tSlot.iVolatileFlags &= ~iFlag;
}

void CBattler::Tick_Volatile_Turns()
{
}

void CBattler::Consume_PP(_uint iIndex)
{
	if (nullptr == m_pInstance || iIndex >= g_kMaxMovesPerPokemon)
		return;

	if (m_pInstance->iCurrentPP[iIndex] > 0)
		--m_pInstance->iCurrentPP[iIndex];
}

void CBattler::Reset_For_Switch(POKEMON_INSTANCE* pNewInstance)
{
	if (nullptr == pNewInstance)
		return;

	m_pInstance = pNewInstance;

	Reset_BattleSlot(m_tSlot);
	m_tSlot.pPokemon = m_pInstance;
}

CBattler* CBattler::Create(const BATTLER_DESC& tDesc)
{
	CBattler* pInstance = new CBattler();

	if (FAILED(pInstance->Initialize(tDesc)))
	{
		MSG_BOX("Failed to Created : CBattler");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattler::Free()
{
	m_pInstance = nullptr;

	__super::Free();
}