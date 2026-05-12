#pragma once
#include "Base.h"
#include "Damage_Pipe.h"
#include "Battle_Context.h"

NS_BEGIN(Game_PKM)

class IDamageModifier;

class CDamage_Calculator final : public CBase
{
private:
	CDamage_Calculator();
	virtual ~CDamage_Calculator() = default;

public:
	HRESULT Initialize();

	HRESULT Add_Modifier(IDamageModifier* pModifier);
	HRESULT Insert_Modifier(_uint iIndex, IDamageModifier* pModifier);
	void Clear_Modifiers();

	void Calculate(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe);

private:
	std::vector<IDamageModifier*> m_vModifiers;

public:
	static CDamage_Calculator* Create();

private:
	virtual void Free() override;
};

NS_END