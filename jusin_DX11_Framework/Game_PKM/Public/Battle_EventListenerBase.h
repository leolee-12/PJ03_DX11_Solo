#pragma once
#include "IBattleEventListener.h"

NS_BEGIN(Game_PKM)

class CBattle_EventListenerBase abstract : public IBattleEventListener
{
public:
	virtual void On_BattleEvent(const BATTLE_EVENT_BASE& tEvent) override
	{
		switch (tEvent.eType)
		{
		case BATTLE_EVENT_TYPE::BATTLE_STARTED:
			On_BattleStarted(static_cast<const EVENT_BATTLE_STARTED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::TURN_STARTED:
			On_TurnStarted(static_cast<const EVENT_TURN_STARTED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::COMMAND_SELECTED:
			On_CommandSelected(static_cast<const EVENT_COMMAND_SELECTED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::MOVE_USED:
			On_MoveUsed(static_cast<const EVENT_MOVE_USED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::MOVE_FAILED:
			On_MoveFailed(static_cast<const EVENT_MOVE_FAILED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::DAMAGE_DEALT:
			On_DamageDealt(static_cast<const EVENT_DAMAGE_DEALT&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::STATUS_APPLIED:
			On_StatusApplied(static_cast<const EVENT_STATUS_APPLIED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::STAT_STAGE_CHANGED:
			On_StatStageChanged(static_cast<const EVENT_STAT_STAGE_CHANGED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::POKEMON_FAINTED:
			On_PokemonFainted(static_cast<const EVENT_POKEMON_FAINTED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::POKEMON_SWITCHED:
			On_PokemonSwitched(static_cast<const EVENT_POKEMON_SWITCHED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::FIELD_CHANGED:
			On_FieldChanged(static_cast<const EVENT_FIELD_CHANGED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::RUN_FAILED:
			On_RunFailed(static_cast<const EVENT_RUN_FAILED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::RUN_SUCCEEDED:
			On_RunSucceeded(static_cast<const EVENT_RUN_SUCCEEDED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::TURN_ENDED:
			On_TurnEnded(static_cast<const EVENT_TURN_ENDED&>(tEvent));
			break;

		case BATTLE_EVENT_TYPE::BATTLE_ENDED:
			On_BattleEnded(static_cast<const EVENT_BATTLE_ENDED&>(tEvent));
			break;

		default:
			break;
		}
	}

	virtual void On_BattleStarted(const EVENT_BATTLE_STARTED&) {}
	virtual void On_TurnStarted(const EVENT_TURN_STARTED&) {}
	virtual void On_CommandSelected(const EVENT_COMMAND_SELECTED&) {}
	virtual void On_MoveUsed(const EVENT_MOVE_USED&) {}
	virtual void On_MoveFailed(const EVENT_MOVE_FAILED&) {}
	virtual void On_DamageDealt(const EVENT_DAMAGE_DEALT&) {}
	virtual void On_StatusApplied(const EVENT_STATUS_APPLIED&) {}
	virtual void On_StatStageChanged(const EVENT_STAT_STAGE_CHANGED&) {}
	virtual void On_PokemonFainted(const EVENT_POKEMON_FAINTED&) {}
	virtual void On_PokemonSwitched(const EVENT_POKEMON_SWITCHED&) {}
	virtual void On_FieldChanged(const EVENT_FIELD_CHANGED&) {}
	virtual void On_RunFailed(const EVENT_RUN_FAILED&) {}
	virtual void On_RunSucceeded(const EVENT_RUN_SUCCEEDED&) {}
	virtual void On_TurnEnded(const EVENT_TURN_ENDED&) {}
	virtual void On_BattleEnded(const EVENT_BATTLE_ENDED&) {}
};

NS_END