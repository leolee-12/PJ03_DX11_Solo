#include "Game_BattleStream.h"

NS_BEGIN(Game_PKM)

void Serialize(IStream& Stream, POKEMON_INSTANCE& v)
{
	Serialize(Stream, v.iSpeciesID);
	Stream.IO(v.szNickname, sizeof(v.szNickname));

	Serialize(Stream, v.iLevel);
	Serialize(Stream, v.iExp);

	Stream.IO(v.iIV, sizeof(v.iIV));
	Stream.IO(v.iEV, sizeof(v.iEV));

	Stream.IO(&v.eNature, sizeof(v.eNature));
	Serialize(Stream, v.iAbilityID);

	Stream.IO(v.iMoves, sizeof(v.iMoves));
	Stream.IO(v.iCurrentPP, sizeof(v.iCurrentPP));

	Serialize(Stream, v.iCurrentHP);
	Stream.IO(&v.eStatus, sizeof(v.eStatus));
	Serialize(Stream, v.iHeldItemID);

	/* iStat은 캐시이므로 저장하지 않는다. 로드 후 Recalc_All_Stats로 재계산. */

	Serialize(Stream, v.iOriginalTrainerID);
	Serialize(Stream, v.iCapturedAtZoneID);
}

void Serialize(IStream& Stream, PARTY& v)
{
	Serialize(Stream, v.iCount);

	for (_ubyte i = 0; i < g_kMaxPartySize; ++i)
		Serialize(Stream, v.arrSlots[i]);
}

void Serialize(IStream& Stream, TRAINER_DATA& v)
{
	Serialize(Stream, v.iTrainerID);
	Stream.IO(v.szName, sizeof(v.szName));

	Stream.IO(&v.eAIType, sizeof(v.eAIType));
	Serialize(Stream, v.tParty);

	Serialize(Stream, v.iRewardMoney);
	Serialize(Stream, v.iRewardItemID);

	Serialize(Stream, v.iEncounterDialogID);
	Serialize(Stream, v.iVictoryDialogID);
	Serialize(Stream, v.iDefeatDialogID);
}

NS_END