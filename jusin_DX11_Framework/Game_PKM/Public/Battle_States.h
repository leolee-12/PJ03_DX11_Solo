#pragma once
#include "IBattleState.h"

NS_BEGIN(Game_PKM)

class IBattleCommand;

class CIntroState final : public IBattleState
{
private:
	CIntroState();
	virtual ~CIntroState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::INTRO; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CIntroState* Create();

private:
	virtual void Free() override;
};

class CInputPlayerState final : public IBattleState
{
private:
	CInputPlayerState();
	virtual ~CInputPlayerState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::INPUT_PLAYER; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CInputPlayerState* Create();

private:
	virtual void Free() override;
};

class CInputOpponentState final : public IBattleState
{
private:
	CInputOpponentState();
	virtual ~CInputOpponentState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::INPUT_OPPONENT; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CInputOpponentState* Create();

private:
	virtual void Free() override;
};

class CResolveOrderState final : public IBattleState
{
private:
	CResolveOrderState();
	virtual ~CResolveOrderState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::RESOLVE_ORDER; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CResolveOrderState* Create();

private:
	virtual void Free() override;
};

class CResolveActionState final : public IBattleState
{
private:
	CResolveActionState();
	virtual ~CResolveActionState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::RESOLVE_ACTION_1; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

private:
	IBattleCommand* m_pCommand = { nullptr };

public:
	static CResolveActionState* Create();

private:
	virtual void Free() override;
};

class CResolveEndTurnState final : public IBattleState
{
private:
	CResolveEndTurnState();
	virtual ~CResolveEndTurnState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::RESOLVE_END_TURN; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CResolveEndTurnState* Create();

private:
	virtual void Free() override;
};

class CCheckEndState final : public IBattleState
{
private:
	CCheckEndState();
	virtual ~CCheckEndState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::CHECK_END; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CCheckEndState* Create();

private:
	virtual void Free() override;
};

class CForcedSwitchState final : public IBattleState
{
private:
	CForcedSwitchState();
	virtual ~CForcedSwitchState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::CHECK_END; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CForcedSwitchState* Create();

private:
	virtual void Free() override;
};

class COutroState final : public IBattleState
{
private:
	COutroState();
	virtual ~COutroState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::OUTRO; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static COutroState* Create();

private:
	virtual void Free() override;
};

class CDoneState final : public IBattleState
{
private:
	CDoneState();
	virtual ~CDoneState() = default;

public:
	virtual BATTLE_PHASE Get_Phase() const override { return BATTLE_PHASE::DONE; }

	virtual void OnEnter(const BATTLE_CONTEXT& ctx) override;
	virtual void Update(const BATTLE_CONTEXT& ctx, _float fTimeDelta) override;
	virtual void OnExit(const BATTLE_CONTEXT& ctx) override;

public:
	static CDoneState* Create();

private:
	virtual void Free() override;
};

NS_END