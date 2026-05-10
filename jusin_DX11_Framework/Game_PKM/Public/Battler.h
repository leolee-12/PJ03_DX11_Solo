#pragma once
#include "Base.h"
#include "Battle_Session.h"

NS_BEGIN(Game_PKM)

class CBattler final : public CBase
{
public:
	struct BATTLER_DESC
	{
		_uint iSide = { g_kBattleSide_Player };
		_uint iSlotIndex = { 0 };
		POKEMON_INSTANCE* pInstance = { nullptr }; // weak
	};

private:
	CBattler();
	virtual ~CBattler() = default;

public:
	HRESULT Initialize(const BATTLER_DESC& tDesc);

	_uint Get_Side() const { return m_iSide; }
	_uint Get_SlotIndex() const { return m_iSlotIndex; }
	POKEMON_INSTANCE* Get_Instance() const { return m_pInstance; }
	const BATTLE_SLOT& Get_Slot() const { return m_tSlot; }

	_ushort Get_CurrentHP() const;
	_ushort Get_MaxHP() const;
	_bool Is_Alive() const { return Get_CurrentHP() > 0; }
	STATUS_CONDITION Get_Status() const;
	_uint Get_MoveID(_uint iIndex) const;
	_ubyte Get_PP(_uint iIndex) const;
	_ushort Get_Stat(STAT eStat) const;
	_byte Get_StatStage(STAGE_INDEX eIndex) const;
	_bool Has_Volatile(_uint iFlag) const { return 0 != (m_tSlot.iVolatileFlags & iFlag); }

	_ushort Apply_Damage(_ushort iAmount);
	_ushort Apply_Heal(_ushort iAmount);
	_bool Set_Status(STATUS_CONDITION eStatus);
	void Clear_Status();
	_byte Modify_StatStage(STAGE_INDEX eIndex, _byte iDelta);
	void Set_Volatile_Flag(_uint iFlag, _ubyte iTurns);
	void Clear_Volatile_Flag(_uint iFlag);
	void Tick_Volatile_Turns();
	void Consume_PP(_uint iIndex);

	void Set_LastMoveUsed(_uint iMoveID) { m_tSlot.iLastMoveUsed = iMoveID; }
	void Set_Protected(_bool b) { m_tSlot.bProtected = b; }
	void Set_MustRecharge(_bool b) { m_tSlot.bMustRecharge = b; }

	void Reset_For_Switch(POKEMON_INSTANCE* pNewInstance);

private:
	_uint m_iSide = { g_kBattleSide_Player };
	_uint m_iSlotIndex = { 0 };
	POKEMON_INSTANCE* m_pInstance = { nullptr }; // weak
	BATTLE_SLOT m_tSlot = {};

public:
	static CBattler* Create(const BATTLER_DESC& tDesc);

private:
	virtual void Free() override;
};

NS_END