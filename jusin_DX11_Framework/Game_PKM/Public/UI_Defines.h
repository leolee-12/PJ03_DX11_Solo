#pragma once
#include "Game_PKM_Defines.h"

NS_BEGIN(Game_PKM)

struct UI_TEXTURE_OPTION
{
	const _char* pLabel;
	WNameID			strTag;

	const _char* pProtoTag;
	const _tchar* pTextureFilePath;
	const _tchar* pDebugName;
	LEVEL			eLevel;
	_uint			iNumTextures;
};

#define UI_TEXTURE_OPTION_ROW(label, tag, path, proto, level, count) \
          { label, tag, proto, path, TEXT(proto), level, count }

inline constexpr UI_TEXTURE_OPTION g_UITextureOptions[] =
{
	// LOGO
	UI_TEXTURE_OPTION_ROW("Title pbgf Diff", PROTO_COM_TEX_TITLE_PBGF_DIFF, TEXT("../../Resources/UI/title/title_pbgf_00.png"),
	  "Prototype_Component_Texture_Title_pbgf_Diff", LEVEL::LOGO, 1u),
	UI_TEXTURE_OPTION_ROW("Title Logo Diff", PROTO_COM_TEX_TITLE_LOGO_DIFF, TEXT("../../Resources/UI/title/title_logo_%02d.png"),
	  "Prototype_Component_Texture_Title_Logo_Diff", LEVEL::LOGO, 3u),
	UI_TEXTURE_OPTION_ROW("Title pbtn Diff", PROTO_COM_TEX_TITLE_PBTN_DIFF, TEXT("../../Resources/UI/title/title_pbtn_%02d.png"),
	  "Prototype_Component_Texture_Title_pbtn_Diff", LEVEL::LOGO, 1u),
	UI_TEXTURE_OPTION_ROW("Title Pika", PROTO_COM_TEX_TITLE_PIKA, TEXT("../../Resources/UI/title/pber_pika_%02d.png"),
	  "Prototype_Component_Texture_Title_Pika", LEVEL::LOGO, 11u),
	UI_TEXTURE_OPTION_ROW("Star", PROTO_COM_TEX_STAR, TEXT("../../Resources/Effects/Star/Star_%02d.png"),
		"Prototype_Component_Texture_Star", LEVEL::LOGO, 3u),
	UI_TEXTURE_OPTION_ROW("Title BackGround", PROTO_COM_TEX_TITLE_BG, TEXT("../../Resources/UI/title/Title_BG.png"),
	  "Prototype_Component_Texture_Title_BackGround", LEVEL::LOGO, 1u),
	UI_TEXTURE_OPTION_ROW("BackGround", PROTO_COM_TEX_BACKGROUND, TEXT("../../Resources/Textures/Default%d.jpg"),
	  "Prototype_Component_Texture_BackGround", LEVEL::LOGO, 2u),

	// STATIC
	UI_TEXTURE_OPTION_ROW("Cursor", PROTO_COM_TEX_CURSOR, TEXT("../../Resources/UI/cursor/cursor.png"),
		"Prototype_Component_Texture_Cursor", LEVEL::STATIC, 1u),
	UI_TEXTURE_OPTION_ROW("Get Text LV", PROTO_COM_TEX_GET_TEXT_LV, TEXT("../../Resources/UI/poke_get/poke_get_text_Lv.png"),
		"Prototype_Component_Texture_Get_Text_LV", LEVEL::STATIC, 1u),
	UI_TEXTURE_OPTION_ROW("Get Text Number", PROTO_COM_TEX_GET_TEXT_NUM, TEXT("../../Resources/UI/poke_get/poke_get_itm_number_%02d.png"),
		"Prototype_Component_Texture_Get_Text_Number", LEVEL::STATIC, 10u),

	// GAMEPLAY - Main Menu
	UI_TEXTURE_OPTION_ROW("Menu Ball", PROTO_COM_TEX_MENU_BALL, TEXT("../../Resources/UI/mainmenu/main_menu_ball.png"),
		"Prototype_Component_Texture_Menu_Ball", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Menu Partner", PROTO_COM_TEX_MENU_PARTNER, TEXT("../../Resources/UI/mainmenu/main_menu_partner_%02d.png"),
		"Prototype_Component_Texture_Menu_Partner", LEVEL::GAMEPLAY, 3u),
	UI_TEXTURE_OPTION_ROW("Menu Dex", PROTO_COM_TEX_MENU_DEX, TEXT("../../Resources/UI/mainmenu/main_menu_dex_%02d.png"),
		"Prototype_Component_Texture_Menu_Dex", LEVEL::GAMEPLAY, 5u),
	UI_TEXTURE_OPTION_ROW("Menu Bag", PROTO_COM_TEX_MENU_BAG, TEXT("../../Resources/UI/mainmenu/main_menu_bag_%02d.png"),
		"Prototype_Component_Texture_Menu_Bag", LEVEL::GAMEPLAY, 5u),
	UI_TEXTURE_OPTION_ROW("Menu Entry", PROTO_COM_TEX_MENU_ENTRY, TEXT("../../Resources/UI/mainmenu/main_menu_entry_%02d.png"),
		"Prototype_Component_Texture_Menu_Entry", LEVEL::GAMEPLAY, 5u),
	UI_TEXTURE_OPTION_ROW("Menu Link", PROTO_COM_TEX_MENU_LINK, TEXT("../../Resources/UI/mainmenu/main_menu_link_%02d.png"),
		"Prototype_Component_Texture_Menu_Link", LEVEL::GAMEPLAY, 5u),
	UI_TEXTURE_OPTION_ROW("Menu Report", PROTO_COM_TEX_MENU_REPORT, TEXT("../../Resources/UI/mainmenu/main_menu_report_%02d.png"),
		"Prototype_Component_Texture_Menu_Report", LEVEL::GAMEPLAY, 5u),

	// GAMEPLAY - Battle Fade
	UI_TEXTURE_OPTION_ROW("Battle Fade Color", PROTO_COM_TEX_BTL_FADE_COL, TEXT("../../Resources/Effects/Fade_Battle/trainer_bg_col.png"),
		"Prototype_Component_Texture_Battle_Fade_Color", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle Fade Noise", PROTO_COM_TEX_BTL_FADE_NOISE, TEXT("../../Resources/Effects/Fade_Battle/trainer_bg_noise.png"),
		"Prototype_Component_Texture_Battle_Fade_Noise", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle Fade Out", PROTO_COM_TEX_BTL_FADE_OUT, TEXT("../../Resources/Effects/Fade_Battle/trainer_bg_out.png"),
		"Prototype_Component_Texture_Battle_Fade_Out", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle Fade Line", PROTO_COM_TEX_BTL_FADE_LINE, TEXT("../../Resources/Effects/Fade_Battle/trainer_line.png"),
		"Prototype_Component_Texture_Battle_Fade_Line", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle Fade Mask", PROTO_COM_TEX_BTL_FADE_MASK, TEXT("../../Resources/Effects/Fade_Battle/trainer_ptn_%02d.png"),
		"Prototype_Component_Texture_Battle_Fade_Mask", LEVEL::GAMEPLAY, 39u),

	// GAMEPLAY - Entry / Pokemon Icon / Loading / MsgBox / Status Gauge
	UI_TEXTURE_OPTION_ROW("Entry BG Plate", PROTO_COM_TEX_ENTRY_BG_PLATE, TEXT("../../Resources/UI/entry/Entry_BG_Plate_%02d.png"),
		"Prototype_Component_Texture_Entry_BG_Plate", LEVEL::GAMEPLAY, 4u),
	UI_TEXTURE_OPTION_ROW("Entry Icon", PROTO_COM_TEX_ENTRY_ICON, TEXT("../../Resources/UI/entry/Entry_Icon.png"),
		"Prototype_Component_Texture_Entry_Icon", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Entry Plate", PROTO_COM_TEX_ENTRY_PLATE, TEXT("../../Resources/UI/entry/Entry_Plate_%02d.png"),
		"Prototype_Component_Texture_Entry_Plate", LEVEL::GAMEPLAY, 3u),
	UI_TEXTURE_OPTION_ROW("Pokemon Icon", PROTO_COM_TEX_POKEMON_ICON, TEXT("../../Resources/UI/icon_pokemon/poke_icon_%03d.png"),
		"Prototype_Component_Texture_Pokemon_Icon", LEVEL::GAMEPLAY, 154u),
	UI_TEXTURE_OPTION_ROW("Loading Mark", PROTO_COM_TEX_LOADING_MARK, TEXT("../../Resources/UI/loading/gokigen_mark_p_%02d.png"),
		"Prototype_Component_Texture_Loading_Mark", LEVEL::GAMEPLAY, 15u),
	UI_TEXTURE_OPTION_ROW("MsgBox BG", PROTO_COM_TEX_MSGBOX_BG, TEXT("../../Resources/UI/msgBox/msgBox_BG_%02d.png"),
		"Prototype_Component_Texture_MsgBox_BG", LEVEL::GAMEPLAY, 2u),
	UI_TEXTURE_OPTION_ROW("MsgBox Icon", PROTO_COM_TEX_MSGBOX_ICON, TEXT("../../Resources/UI/msgBox/msgBox_Icon.png"),
		"Prototype_Component_Texture_MsgBox_Icon", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Status Gauge Back", PROTO_COM_TEX_STATUS_GAUGE_BACK, TEXT("../../Resources/UI/status/gauge_back.png"),
		"Prototype_Component_Texture_Status_Gauge_Back", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Status Gauge Frame", PROTO_COM_TEX_STATUS_GAUGE_FRAME, TEXT("../../Resources/UI/status/gauge_frame.png"),
		"Prototype_Component_Texture_Status_Gauge_Frame", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Status Gauge Outline", PROTO_COM_TEX_STATUS_GAUGE_OUTLINE, TEXT("../../Resources/UI/status/gauge_outline.png"),
		"Prototype_Component_Texture_Status_Gauge_Outline", LEVEL::GAMEPLAY, 1u),

	// GAMEPLAY - Battle Buttons / Plates / Number / Status / Move
	UI_TEXTURE_OPTION_ROW("Battle Button Fight", PROTO_COM_TEX_BTL_BTN_FIGHT, TEXT("../../Resources/UI/poke_battle/battle_command_battle_%02d.png"),
		"Prototype_Component_Texture_Battle_Button_Fight", LEVEL::GAMEPLAY, 4u),
	UI_TEXTURE_OPTION_ROW("Battle Button Poke", PROTO_COM_TEX_BTL_BTN_POKE, TEXT("../../Resources/UI/poke_battle/battle_command_poke_%02d.png"),
		"Prototype_Component_Texture_Battle_Button_Poke", LEVEL::GAMEPLAY, 4u),
	UI_TEXTURE_OPTION_ROW("Battle Button Bag", PROTO_COM_TEX_BTL_BTN_BAG, TEXT("../../Resources/UI/poke_battle/battle_command_bag_%02d.png"),
		"Prototype_Component_Texture_Battle_Button_Bag", LEVEL::GAMEPLAY, 4u),
	UI_TEXTURE_OPTION_ROW("Battle PlayerPlate", PROTO_COM_TEX_BTL_PLAYERPLATE, TEXT("../../Resources/UI/poke_battle/battle_PlayerPlate.png"),
		"Prototype_Component_Texture_Battle_PlayerPlate", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle EnemyPlate", PROTO_COM_TEX_BTL_ENEMYPLATE, TEXT("../../Resources/UI/poke_battle/battle_EnemyPlate.png"),
		"Prototype_Component_Texture_Battle_EnemyPlate", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle BallPlate", PROTO_COM_TEX_BTL_BALLPLATE, TEXT("../../Resources/UI/poke_battle/battle_BallPlate.png"),
		"Prototype_Component_Texture_Battle_BallPlate", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle BallIcon", PROTO_COM_TEX_BTL_BALLICON, TEXT("../../Resources/UI/poke_battle/ball_icon_%02d.png"),
		"Prototype_Component_Texture_Battle_BallIcon", LEVEL::GAMEPLAY, 4u),
	UI_TEXTURE_OPTION_ROW("Battle Number", PROTO_COM_TEX_BTL_NUM, TEXT("../../Resources/UI/poke_battle/battle_Num_%02d.png"),
		"Prototype_Component_Texture_Battle_Number", LEVEL::GAMEPLAY, 10u),
	UI_TEXTURE_OPTION_ROW("Battle Number Slash", PROTO_COM_TEX_BTL_NUM_SLASH, TEXT("../../Resources/UI/poke_battle/battle_Num_slash.png"),
		"Prototype_Component_Texture_Battle_Number_Slash", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Status Gender", PROTO_COM_TEX_STATUS_GENDER, TEXT("../../Resources/UI/poke_battle/status_gender_%02d.png"),
		"Prototype_Component_Texture_Status_Gender", LEVEL::GAMEPLAY, 2u),
	UI_TEXTURE_OPTION_ROW("Battle MsgBox", PROTO_COM_TEX_BTL_MSGBOX, TEXT("../../Resources/UI/poke_battleMsg/Battle_MsgBox.png"),
		"Prototype_Component_Texture_Battle_MsgBox", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle MsgIcon", PROTO_COM_TEX_BTL_MSGICON, TEXT("../../Resources/UI/poke_battleMsg/Battle_MsgIcon.png"),
		"Prototype_Component_Texture_Battle_MsgIcon", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Battle Move", PROTO_COM_TEX_BTL_MOVE, TEXT("../../Resources/UI/poke_battleMove/BattleMove_%02d.png"),
		"Prototype_Component_Texture_Battle_Move", LEVEL::GAMEPLAY, 36u),
	UI_TEXTURE_OPTION_ROW("Battle MoveIcon", PROTO_COM_TEX_BTL_MOVEICON, TEXT("../../Resources/UI/poke_battleMove/BattleMoveIcon_%02d.png"),
		"Prototype_Component_Texture_Battle_MoveIcon", LEVEL::GAMEPLAY, 18u),
	UI_TEXTURE_OPTION_ROW("Battle MoveShadow", PROTO_COM_TEX_BTL_MOVESHADOW, TEXT("../../Resources/UI/poke_battleMove/BattleMove_Shadow.png"),
		"Prototype_Component_Texture_Battle_MoveShadow", LEVEL::GAMEPLAY, 1u),

	// GAMEPLAY - Capture (Get)
	UI_TEXTURE_OPTION_ROW("Get Button", PROTO_COM_TEX_GET_BUTTON, TEXT("../../Resources/UI/poke_get/poke_get_button_%02d.png"),
		"Prototype_Component_Texture_Get_Button", LEVEL::GAMEPLAY, 3u),
	UI_TEXTURE_OPTION_ROW("Get Icon", PROTO_COM_TEX_GET_ICON, TEXT("../../Resources/UI/poke_get/poke_get_icon.png"),
		"Prototype_Component_Texture_Get_Icon", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Get Info", PROTO_COM_TEX_GET_INFO, TEXT("../../Resources/UI/poke_get/poke_get_Info.png"),
		"Prototype_Component_Texture_Get_Info", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Get Info2", PROTO_COM_TEX_GET_INFO2, TEXT("../../Resources/UI/poke_get/poke_get_Info2.png"),
		"Prototype_Component_Texture_Get_Info2", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Get Line Fill", PROTO_COM_TEX_GET_LINE_FILL, TEXT("../../Resources/UI/poke_get/poke_get_info_line_fill.png"),
		"Prototype_Component_Texture_Get_Line_Fill", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Get Line Back", PROTO_COM_TEX_GET_LINE_BACK, TEXT("../../Resources/UI/poke_get/poke_get_info_line_back.png"),
		"Prototype_Component_Texture_Get_Line_Back", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Get Line2 Fill", PROTO_COM_TEX_GET_LINE2_FILL, TEXT("../../Resources/UI/poke_get/poke_get_info_line2_fill.png"),
		"Prototype_Component_Texture_Get_Line2_Fill", LEVEL::GAMEPLAY, 1u),
	UI_TEXTURE_OPTION_ROW("Get Line2 Back", PROTO_COM_TEX_GET_LINE2_BACK, TEXT("../../Resources/UI/poke_get/poke_get_info_line2_back.png"),
		"Prototype_Component_Texture_Get_Line2_Back", LEVEL::GAMEPLAY, 1u),
};

#undef UI_TEXTURE_OPTION_ROW

inline const UI_TEXTURE_OPTION* UI_FindTextureOption(WNameID strTag)
{
	for (const auto& option : g_UITextureOptions)
	{
		if (option.strTag == strTag)
			return &option;
	}

	return nullptr;
}

inline _string UI_TextureTagToString(WNameID strTag)
{
	if (const UI_TEXTURE_OPTION* pOption = UI_FindTextureOption(strTag))
		return pOption->pProtoTag;

#ifdef _DEBUG
	const _string strLookup = WtoS(_wstring(Engine::WNameRegistry::Lookup(strTag)));
	return (strLookup == "<unknown>") ? _string{} : strLookup;
#else
	return {};
#endif
}

NS_END