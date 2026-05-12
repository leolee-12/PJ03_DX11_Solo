#pragma once
#include "Base.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

class IBattleCommand;

class CCommandQueue final : public CBase
{
private:
	CCommandQueue();
	virtual ~CCommandQueue() = default;

public:
	HRESULT Initialize();

	HRESULT Push(IBattleCommand* pCmd);
	IBattleCommand* Pop();

	void Sort(const BATTLE_CONTEXT& ctx);
	void Clear();

	_bool Empty() const { return m_vCommands.empty(); }
	size_t Size() const { return m_vCommands.size(); }

private:
	static _byte Resolve_Priority(IBattleCommand* pCmd, const BATTLE_CONTEXT& ctx);
	static _ushort Resolve_Speed(IBattleCommand* pCmd, const BATTLE_CONTEXT& ctx);

private:
	std::vector<IBattleCommand*> m_vCommands;

public:
	static CCommandQueue* Create();

private:
	virtual void Free() override;
};

NS_END