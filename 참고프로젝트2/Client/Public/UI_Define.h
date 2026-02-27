#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CUI_Define
{
public:
	enum PLAYER_SLOT { LPLAYER1, LPLAYER2, RPLAYER1, RPLAYER2, SLOT_END };
	enum PLAYER_ID { GOKU, ANDROID21, FRIEZA, HIT, PAWN_END };
	enum UI_LISTPOS { TOP, MID, BOT, LIST_END };
	enum NPC_ID { NPC_FRIEZA, NPC_KRILLIN, NPC_GOKU, ID_END };
};

END

