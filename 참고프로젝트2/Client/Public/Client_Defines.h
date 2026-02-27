#pragma once

#include <process.h>

namespace Client
{
	const unsigned int	g_iWinSizeX = { 1920 };
	const unsigned int	g_iWinSizeY = { 1080 };


	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_LOBBY, LEVEL_CHARACTER, LEVEL_VS, LEVEL_GAMEPLAY, LEVEL_END };

	//enum CCharacter::PLAYER_SLOT { CCharacter::LPLAYER2,CCharacter::LPLAYER2, CCharacter::RPLAYER1, CCharacter::RPLAYER2, CCharacter::PLAYER_SLOT::SLOT_END };
	//enum PLAYER_ID { GOGU, ANDROID21, BUU, HIT, PAWN_END };

	enum EFFECT_TYPE { EFFECT_NONELIGHT, EFFECT_BLEND, EFFECT_ZNONE, EFFECT_OVERLAP, EFFECT_END };
}

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

using namespace Client;
