#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton_Layered.h"
#include "UIButton_Glow.h"
#include "UIButton_Entry.h"

NS_BEGIN(Game_PKM)

enum class GLOW_BUTTON_PRESET
{
    MENU_PARTNER,
    MENU_SQUARE,
    END
};

enum class LAYERED_BUTTON_PRESET
{
    GET_COMMAND,
    BATTLE_COMMAND,
    END
};

enum class ENTRY_BUTTON_PRESET
{
    PLATE,
    END
};

CUIButton_Glow::GLOWBUTTON_DESC Get_GlowButtonPreset(GLOW_BUTTON_PRESET ePreset);
CUIButton_Layered::LAYEREDBUTTON_DESC Get_LayeredButtonPreset(LAYERED_BUTTON_PRESET ePreset);
CUIButton_Entry::ENTRYBUTTON_DESC Get_EntryButtonPreset(ENTRY_BUTTON_PRESET ePreset);

NS_END