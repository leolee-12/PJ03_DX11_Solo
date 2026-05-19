#pragma once
#include "IBattleCommand.h"
#include "Camera_Defines.h"

NS_BEGIN(Game_PKM)

class CMoveCommand final : public IBattleCommand
{
public:
	struct DESC
	{
		_uint iActorSide = { g_kBattleSide_Player };
		_uint iActorSlot = { 0 };
		_uint iMoveSlot = { 0 };
		_uint iTargetSide = { g_kBattleSide_Opponent };
		_uint iTargetSlot = { 0 };
		CAMERA_SEQUENCE_ID eCameraSequence = { CAMERA_SEQUENCE_ID::NONE };
	};

private:
	CMoveCommand();
	virtual ~CMoveCommand() = default;

public:
	HRESULT Initialize(const DESC& tDesc);

	virtual ACTION_TYPE Get_Type() const override { return ACTION_TYPE::USE_MOVE; }
	virtual _uint Get_ActorSide() const override { return m_tDesc.iActorSide; }
	virtual _byte Get_Priority() const override { return 0; }
	virtual _ushort Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const override;
	virtual HRESULT Execute(const BATTLE_CONTEXT& ctx) override;

	_uint Get_MoveSlot() const { return m_tDesc.iMoveSlot; }

private:
	DESC m_tDesc = {};

public:
	static CMoveCommand* Create(const DESC& tDesc);

private:
	virtual void Free() override;
};

class CSwitchCommand final : public IBattleCommand
{
public:
	struct DESC
	{
		_uint iActorSide = { g_kBattleSide_Player };
		_uint iActorSlot = { 0 };
		_uint iTargetPartyIndex = { 0 };
	};

private:
	CSwitchCommand();
	virtual ~CSwitchCommand() = default;

public:
	HRESULT Initialize(const DESC& tDesc);

	virtual ACTION_TYPE Get_Type() const override { return ACTION_TYPE::SWITCH; }
	virtual _uint Get_ActorSide() const override { return m_tDesc.iActorSide; }
	virtual _byte Get_Priority() const override { return 6; }
	virtual _ushort Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const override;
	virtual HRESULT Execute(const BATTLE_CONTEXT& ctx) override;

private:
	DESC m_tDesc = {};

public:
	static CSwitchCommand* Create(const DESC& tDesc);

private:
	virtual void Free() override;
};

class CRunCommand final : public IBattleCommand
{
public:
	struct DESC
	{
		_uint iActorSide = { g_kBattleSide_Player };
	};

private:
	CRunCommand();
	virtual ~CRunCommand() = default;

public:
	HRESULT Initialize(const DESC& tDesc);

	virtual ACTION_TYPE Get_Type() const override { return ACTION_TYPE::RUN; }
	virtual _uint Get_ActorSide() const override { return m_tDesc.iActorSide; }
	virtual _byte Get_Priority() const override { return 6; }
	virtual _ushort Get_ActorSpeed(const BATTLE_CONTEXT& ctx) const override;
	virtual HRESULT Execute(const BATTLE_CONTEXT& ctx) override;

private:
	DESC m_tDesc = {};

public:
	static CRunCommand* Create(const DESC& tDesc);

private:
	virtual void Free() override;
};

NS_END