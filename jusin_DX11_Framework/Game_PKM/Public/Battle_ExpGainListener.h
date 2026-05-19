#pragma once
#include "Battle_EventListenerBase.h"

NS_BEGIN(Game_PKM)

class CBattle_Manager;

/* CBattle_ExpGainListener
   - Dispatcher 구독자. 적 사이드 POKEMON_FAINTED 시점에 끼어드는 확장 슬롯.
   - 본 단위에서는 placeholder - Add_Pacing_Lock 후 grace 경과 시 Release.
   - 후속 작업에서 본 리스너 안에 경험치 산식 / 시퀀서 step / UI 호출이 채워짐.
   - CBattle_Manager 는 weak (소유: CLevel_Battle). */
class CBattle_ExpGainListener final : public CBattle_EventListenerBase
{
private:
	CBattle_ExpGainListener();
	virtual ~CBattle_ExpGainListener() = default;

public:
	HRESULT Initialize();
	void Bind(CBattle_Manager* pManager);
	void Tick(_float fTimeDelta);

	virtual void On_PokemonFainted(const EVENT_POKEMON_FAINTED& tEvent) override;

private:
	CBattle_Manager* m_pManager = { nullptr };  // weak

	_bool   m_bLockHeld = { false };
	_float  m_fGraceTimer = { 0.f };

	static constexpr _float s_fGraceSeconds = 0.05f;

public:
	static CBattle_ExpGainListener* Create();

private:
	virtual void Free() override;
};

NS_END