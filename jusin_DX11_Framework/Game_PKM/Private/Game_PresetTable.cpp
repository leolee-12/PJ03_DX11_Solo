#include "Game_PresetTable.h"

NS_BEGIN(Game_PKM)

CUIButton_Glow::GLOWBUTTON_DESC Get_GlowButtonPreset(GLOW_BUTTON_PRESET ePreset)
{
	CUIButton_Glow::GLOWBUTTON_DESC tDesc{};
	tDesc.fGlowPulseSpeed = 6.f;
	tDesc.fGlowFadeSpeed = 8.f;

	switch (ePreset)
	{
	case GLOW_BUTTON_PRESET::MENU_PARTNER:
		tDesc.iBaseTextureIndex = 0;
		tDesc.iHoverTextureIndex = 0;
		tDesc.iGlowTextureIndex = 1;
		tDesc.iDisabledTextureIndex = 2;
		tDesc.iMaskTextureIndex = 1;
		break;

	case GLOW_BUTTON_PRESET::MENU_SQUARE:
		tDesc.iBaseTextureIndex = 0;
		tDesc.iHoverTextureIndex = 1;
		tDesc.iGlowTextureIndex = 2;
		tDesc.iDisabledTextureIndex = 3;
		tDesc.iMaskTextureIndex = 4;
		break;

	default:
		break;
	}

	return tDesc;
}

CUIButton_Layered::LAYEREDBUTTON_DESC Get_LayeredButtonPreset(LAYERED_BUTTON_PRESET ePreset)
{
	CUIButton_Layered::LAYEREDBUTTON_DESC tDesc{};

	switch (ePreset)
	{
	case LAYERED_BUTTON_PRESET::GET_COMMAND:
		tDesc.vColBase_Normal = _float4{ 1.f, 1.f, 1.f, 1.f };
		tDesc.vColLine_Normal = _float4{ 1.f, 1.f, 0.f, 1.f };
		tDesc.vColBase_Hover = _float4{ 1.f, 1.f, 0.f, 1.f };
		tDesc.vColLine_Hover = _float4{ 1.f, 1.f, 1.f, 1.f };

		tDesc.bUseGlow = true;
		tDesc.bUseMirrorUV = true;
		tDesc.fGlowPulseSpeed = 6.f;
		tDesc.fGlowFadeSpeed = 8.f;
		break;

	default:
		break;
	}

	return tDesc;
}

NS_END