#pragma once
#include "IDamageModifier.h"

NS_BEGIN(Game_PKM)

#define DECLARE_DAMAGE_MODIFIER(CLASSNAME, TAG)                                         \
  class CLASSNAME final : public IDamageModifier                                        \
  {                                                                                     \
  private:                                                                              \
        CLASSNAME() = default;                                                          \
        virtual ~CLASSNAME() = default;                                                 \
  public:                                                                               \
        virtual void Apply(const BATTLE_CONTEXT& ctx, DAMAGE_PIPE_DATA& pipe) override; \
        virtual const _tchar* Get_Tag() const override { return TEXT(TAG); }            \
        static CLASSNAME* Create() { return new CLASSNAME(); }                          \
  private:                                                                              \
        virtual void Free() override { __super::Free(); }                               \
  };

DECLARE_DAMAGE_MODIFIER(CStatStageModifier, "StatStage")
DECLARE_DAMAGE_MODIFIER(CTypeChartModifier, "TypeChart")
DECLARE_DAMAGE_MODIFIER(CStabModifier, "STAB")
DECLARE_DAMAGE_MODIFIER(CAbilityModifier, "Ability")
DECLARE_DAMAGE_MODIFIER(CItemModifier, "Item")
DECLARE_DAMAGE_MODIFIER(CWeatherModifier, "Weather")
DECLARE_DAMAGE_MODIFIER(CFieldModifier, "Field")
DECLARE_DAMAGE_MODIFIER(CCritModifier, "Crit")
DECLARE_DAMAGE_MODIFIER(CRandomRollModifier, "RandomRoll")

#undef DECLARE_DAMAGE_MODIFIER

NS_END