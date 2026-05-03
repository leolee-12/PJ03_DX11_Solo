#pragma once
#include "Game_PKM_Defines.h"
#include "UIButton_Layered.h"
#include "UIButton_Glow.h"

NS_BEGIN(Game_PKM)

enum class GLOW_BUTTON_PRESET
{
	MENU,
	END
};

enum class LAYERED_BUTTON_PRESET
{
	GET_COMMAND,
	END
};

CUIButton_Glow::GLOWBUTTON_DESC Get_GlowButtonPreset(GLOW_BUTTON_PRESET ePreset);
CUIButton_Layered::LAYEREDBUTTON_DESC Get_LayeredButtonPreset(LAYERED_BUTTON_PRESET ePreset);

NS_END